#ifndef HTPSTE_MONTE_CARLO_SCENARIO_GENERATOR_HPP
#define HTPSTE_MONTE_CARLO_SCENARIO_GENERATOR_HPP

#include "htpste/market/market_model.hpp"
#include "htpste/scenarios/monte_carlo_scenario.hpp"

#include <Eigen/Core>

#include <cstddef>
#include <cstdint>
#include <vector>

namespace htpste
{
class MonteCarloScenarioGenerator final
{
  public:
    explicit MonteCarloScenarioGenerator(MarketModel market_model);

    [[nodiscard]] const MarketModel& marketModel() const noexcept;
    [[nodiscard]] std::vector<MonteCarloScenario>
    generate(std::size_t scenario_count, std::uint64_t seed,
             std::size_t thread_count = 1) const;

  private:
    [[nodiscard]] PriceShockMap
    generatePriceShocks(std::size_t scenario_id, std::uint64_t seed) const;

    MarketModel m_market_model;
    Eigen::MatrixXd m_cholesky_factor;
};

} // namespace htpste

#endif // HTPSTE_MONTE_CARLO_SCENARIO_GENERATOR_HPP
