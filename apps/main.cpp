#include "htpste/domain/market_state.hpp"
#include "htpste/domain/portfolio.hpp"
#include "htpste/market/market_model_loader.hpp"
#include "htpste/scenarios/monte_carlo_scenario_generator.hpp"
#include "htpste/simulation/simulation_engine.hpp"

#include <algorithm>
#include <charconv>
#include <chrono>
#include <csignal>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <thread>
#include <utility>
#include <vector>

namespace
{
volatile std::sig_atomic_t stop_requested = 0;

struct CliOptions final
{
    std::filesystem::path market_model_path{"python/market_model.json"};
    std::size_t scenario_count{10'000};
    std::uint64_t seed{42};
    std::size_t thread_count{
        std::max(1U, std::thread::hardware_concurrency())};
    bool repeat{false};
    bool show_help{false};
};

void handleSignal(const int) noexcept
{
    stop_requested = 1;
}

void printUsage(const std::string_view executable_name)
{
    std::cout
        << "Usage: " << executable_name << " [options]\n\n"
        << "Options:\n"
        << "  --market-model <path>  Market model JSON file "
           "(default: python/market_model.json)\n"
        << "  --scenarios <count>    Scenarios per job (default: 10000)\n"
        << "  --seed <value>         Base random seed (default: 42)\n"
        << "  --threads <count>      Scenario generation threads "
           "(default: hardware concurrency)\n"
        << "  --repeat               Run jobs continuously until interrupted\n"
        << "  --help                 Show this help message\n";
}

template <typename Integer>
[[nodiscard]] Integer parseInteger(const std::string_view text,
                                   const std::string_view option_name)
{
    Integer value{};
    const auto [end, error] =
        std::from_chars(text.data(), text.data() + text.size(), value);
    if (error != std::errc{} || end != text.data() + text.size())
    {
        throw std::invalid_argument("invalid value for " +
                                    std::string{option_name} + ": " +
                                    std::string{text});
    }
    return value;
}

[[nodiscard]] std::string_view requireValue(const int argument_count,
                                            char *arguments[], int& index,
                                            const std::string_view option_name)
{
    if (index + 1 >= argument_count)
    {
        throw std::invalid_argument("missing value for " +
                                    std::string{option_name});
    }
    ++index;
    return arguments[index];
}

[[nodiscard]] CliOptions parseOptions(const int argument_count,
                                      char *arguments[])
{
    CliOptions options;
    for (int index = 1; index < argument_count; ++index)
    {
        const std::string_view argument{arguments[index]};
        if (argument == "--market-model")
        {
            options.market_model_path = requireValue(
                argument_count, arguments, index, argument);
        }
        else if (argument == "--scenarios")
        {
            options.scenario_count = parseInteger<std::size_t>(
                requireValue(argument_count, arguments, index, argument),
                argument);
            if (options.scenario_count == 0)
            {
                throw std::invalid_argument(
                    "--scenarios must be greater than zero");
            }
        }
        else if (argument == "--seed")
        {
            options.seed = parseInteger<std::uint64_t>(
                requireValue(argument_count, arguments, index, argument),
                argument);
        }
        else if (argument == "--threads")
        {
            options.thread_count = parseInteger<std::size_t>(
                requireValue(argument_count, arguments, index, argument),
                argument);
            if (options.thread_count == 0)
            {
                throw std::invalid_argument(
                    "--threads must be greater than zero");
            }
        }
        else if (argument == "--repeat")
        {
            options.repeat = true;
        }
        else if (argument == "--help" || argument == "-h")
        {
            options.show_help = true;
        }
        else
        {
            throw std::invalid_argument("unknown option: " +
                                        std::string{argument});
        }
    }
    return options;
}

[[nodiscard]] htpste::MarketState
makeMarketState(const htpste::MarketModel& market_model)
{
    htpste::PriceMap prices;
    prices.reserve(market_model.assetCount());
    const auto symbols = market_model.symbols();
    for (std::size_t index = 0; index < market_model.assetCount(); ++index)
    {
        prices.emplace(symbols[index],
                       market_model.prices()[static_cast<Eigen::Index>(index)]);
    }
    return htpste::MarketState{std::move(prices)};
}

[[nodiscard]] htpste::Portfolio
makeDemonstrationPortfolio(const htpste::MarketModel& market_model)
{
    std::vector<htpste::Position> positions;
    positions.reserve(market_model.assetCount());
    for (std::size_t index = 0; index < market_model.assetCount(); ++index)
    {
        const double quantity = index % 2 == 0 ? 100.0 : -50.0;
        positions.emplace_back(market_model.symbols()[index], quantity);
    }
    return htpste::Portfolio{std::move(positions)};
}

[[nodiscard]] std::vector<const htpste::Scenario *>
scenarioPointers(
    const std::vector<htpste::MonteCarloScenario>& generated_scenarios)
{
    std::vector<const htpste::Scenario *> pointers;
    pointers.reserve(generated_scenarios.size());
    for (const auto& scenario : generated_scenarios)
    {
        pointers.push_back(&scenario);
    }
    return pointers;
}

void printReport(const std::uint64_t job_index, const std::uint64_t job_seed,
                 const std::size_t generation_threads,
                 const std::chrono::nanoseconds generation_runtime,
                 const std::chrono::nanoseconds total_runtime,
                 const htpste::SimulationReport& report)
{
    const auto milliseconds = [](const std::chrono::nanoseconds duration)
    {
        return std::chrono::duration<double, std::milli>{duration}.count();
    };

    const auto& risk = report.riskMetrics();
    std::cout << std::fixed << std::setprecision(2)
              << "\nJob " << job_index << " complete\n"
              << "  Seed: " << job_seed << '\n'
              << "  Scenarios: " << report.scenarioResults().size() << '\n'
              << "  Generation threads: " << generation_threads << '\n'
              << "  Portfolio value: "
              << report.currentPortfolio().totalValue() << '\n'
              << "  VaR (" << risk.confidenceLevel() * 100.0
              << "%): " << risk.valueAtRisk() << '\n'
              << "  CVaR: " << risk.conditionalValueAtRisk() << '\n'
              << "  Worst scenarios:\n";
    for (const auto& scenario : risk.worstScenarios())
    {
        std::cout << "    [" << scenario.scenarioId()
                  << "] P&L = " << scenario.pnl() << '\n';
    }

    const double total_seconds =
        std::chrono::duration<double>{total_runtime}.count();
    const double total_throughput =
        total_seconds == 0.0
            ? 0.0
            : static_cast<double>(report.scenarioResults().size()) /
                  total_seconds;
    std::cout << "  Generation runtime: " << milliseconds(generation_runtime)
              << " ms\n"
              << "  Repricing and risk runtime: "
              << milliseconds(report.runtime()) << " ms\n"
              << "  Total runtime: " << milliseconds(total_runtime) << " ms\n"
              << "  End-to-end throughput: " << total_throughput
              << " scenarios/second\n"
              << std::flush;
}

void runJob(const std::uint64_t job_index, const std::uint64_t job_seed,
            const CliOptions& options,
            const htpste::MonteCarloScenarioGenerator& generator,
            const htpste::Portfolio& portfolio,
            const htpste::MarketState& market_state)
{
    const auto job_started_at = std::chrono::steady_clock::now();
    std::cout << "Starting job " << job_index << " with seed " << job_seed
              << "...\n"
              << std::flush;

    const auto generation_started_at = std::chrono::steady_clock::now();
    const std::vector<htpste::MonteCarloScenario> generated_scenarios =
        generator.generate(options.scenario_count, job_seed,
                           options.thread_count);
    const auto generation_runtime =
        std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::steady_clock::now() - generation_started_at);
    const auto scenario_pointers = scenarioPointers(generated_scenarios);

    const htpste::SimulationReport report = htpste::SimulationEngine{}.run(
        portfolio, market_state, scenario_pointers,
        htpste::SimulationSettings{0.95, 3});
    const auto total_runtime =
        std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::steady_clock::now() - job_started_at);
    printReport(job_index, job_seed, options.thread_count, generation_runtime,
                total_runtime, report);
}

} // namespace

int main(const int argument_count, char *arguments[])
{
    try
    {
        const CliOptions options = parseOptions(argument_count, arguments);
        if (options.show_help)
        {
            printUsage(arguments[0]);
            return 0;
        }

        htpste::MarketModel market_model =
            htpste::MarketModelLoader{}.loadFromFile(
                options.market_model_path);
        const htpste::MarketState market_state =
            makeMarketState(market_model);
        const htpste::Portfolio portfolio =
            makeDemonstrationPortfolio(market_model);
        const htpste::MonteCarloScenarioGenerator generator{
            std::move(market_model)};

        std::signal(SIGINT, handleSignal);
        std::signal(SIGTERM, handleSignal);

        std::uint64_t job_index = 0;
        while (stop_requested == 0)
        {
            runJob(job_index, options.seed + job_index, options, generator,
                   portfolio, market_state);
            ++job_index;
            if (!options.repeat)
            {
                break;
            }
        }

        if (stop_requested != 0)
        {
            std::cout << "Stopping after the current simulation job.\n";
        }
        return 0;
    }
    catch (const std::exception& exception)
    {
        std::cerr << "Simulation failed: " << exception.what() << '\n';
        return 1;
    }
}
