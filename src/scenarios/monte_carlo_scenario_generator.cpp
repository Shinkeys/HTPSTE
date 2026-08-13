#include "htpste/scenarios/monte_carlo_scenario_generator.hpp"

#include <Eigen/Cholesky>

#include <algorithm>
#include <cmath>
#include <exception>
#include <random>
#include <stdexcept>
#include <thread>
#include <utility>

namespace htpste
{
MonteCarloScenarioGenerator::MonteCarloScenarioGenerator(
    MarketModel market_model)
    : m_market_model(std::move(market_model))
{
    const Eigen::LLT<Eigen::MatrixXd> decomposition(
        m_market_model.covariance());
    if (decomposition.info() != Eigen::Success)
    {
        throw std::invalid_argument(
            "covariance matrix cannot be decomposed by Cholesky");
    }
    m_cholesky_factor = decomposition.matrixL();
}

const MarketModel& MonteCarloScenarioGenerator::marketModel() const noexcept
{
    return m_market_model;
}

std::vector<MonteCarloScenario> MonteCarloScenarioGenerator::generate(
    const std::size_t scenario_count, const std::uint64_t seed,
    const std::size_t thread_count) const
{
    if (thread_count == 0)
    {
        throw std::invalid_argument("thread count must be greater than zero");
    }
    if (scenario_count == 0)
    {
        return {};
    }

    const std::size_t worker_count = std::min(thread_count, scenario_count);
    std::vector<PriceShockMap> generated_shocks(scenario_count);
    std::vector<std::exception_ptr> worker_exceptions(worker_count);

    const auto generate_range =
        [this, seed, scenario_count, worker_count, &generated_shocks,
         &worker_exceptions](const std::size_t worker_id)
    {
        const std::size_t scenarios_per_worker = scenario_count / worker_count;
        const std::size_t remainder = scenario_count % worker_count;
        const std::size_t begin = scenarios_per_worker * worker_id +
                                  std::min(worker_id, remainder);
        const std::size_t end = begin + scenarios_per_worker +
                                (worker_id < remainder ? 1U : 0U);
        try
        {
            for (std::size_t scenario_id = begin; scenario_id < end;
                 ++scenario_id)
            {
                generated_shocks[scenario_id] =
                    generatePriceShocks(scenario_id, seed);
            }
        }
        catch (...)
        {
            worker_exceptions[worker_id] = std::current_exception();
        }
    };

    if (worker_count == 1)
    {
        generate_range(0);
    }
    else
    {
        std::vector<std::jthread> workers;
        workers.reserve(worker_count);
        for (std::size_t worker_id = 0; worker_id < worker_count; ++worker_id)
        {
            workers.emplace_back(generate_range, worker_id);
        }
    }

    for (const auto& worker_exception : worker_exceptions)
    {
        if (worker_exception)
        {
            std::rethrow_exception(worker_exception);
        }
    }

    std::vector<MonteCarloScenario> scenarios;
    scenarios.reserve(scenario_count);
    for (std::size_t scenario_id = 0; scenario_id < scenario_count;
         ++scenario_id)
    {
        scenarios.emplace_back(scenario_id,
                               std::move(generated_shocks[scenario_id]));
    }
    return scenarios;
}

PriceShockMap MonteCarloScenarioGenerator::generatePriceShocks(
    const std::size_t scenario_id, const std::uint64_t seed) const
{
    const auto scenario_id_value = static_cast<std::uint64_t>(scenario_id);
    std::seed_seq seed_sequence{
        static_cast<std::uint32_t>(seed),
        static_cast<std::uint32_t>(seed >> 32U),
        static_cast<std::uint32_t>(scenario_id_value),
        static_cast<std::uint32_t>(scenario_id_value >> 32U)};
    std::mt19937_64 random_engine{seed_sequence};
    std::normal_distribution<double> standard_normal{0.0, 1.0};

    const auto asset_count =
        static_cast<Eigen::Index>(m_market_model.assetCount());
    Eigen::VectorXd independent_shocks(asset_count);
    for (Eigen::Index asset_index = 0; asset_index < asset_count;
         ++asset_index)
    {
        independent_shocks[asset_index] = standard_normal(random_engine);
    }

    const Eigen::VectorXd log_returns =
        m_market_model.meanLogReturns() +
        m_cholesky_factor * independent_shocks;
    
    // TODO: rewrite price_shocks not to hold each price shock as a separate entry in unordered_map, but, for example, inside of a matrix
    PriceShockMap price_shocks;
    price_shocks.reserve(m_market_model.assetCount());
    const auto symbols = m_market_model.symbols();
    for (Eigen::Index asset_index = 0; asset_index < asset_count;
         ++asset_index)
    {
        const double relative_shock = std::expm1(log_returns[asset_index]);
        if (!std::isfinite(relative_shock))
        {
            throw std::overflow_error(
                "generated scenario contains a non-finite price shock");
        }
        price_shocks.emplace(symbols[static_cast<std::size_t>(asset_index)],
                             relative_shock);
    }
    return price_shocks;
}

} // namespace htpste
