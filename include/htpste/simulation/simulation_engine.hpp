#ifndef SIMULATION_ENGINE_H
#define SIMULATION_ENGINE_H

#include "htpste/domain/market_state.hpp"
#include "htpste/domain/portfolio.hpp"
#include "htpste/pricing/portfolio_valuation.hpp"
#include "htpste/pricing/scenario_repricing.hpp"
#include "htpste/risk/risk_metrics.hpp"
#include "htpste/scenarios/scenario.hpp"

#include <chrono>
#include <cstddef>
#include <span>
#include <vector>

namespace htpste
{
class SimulationSettings final
{
  public:
    explicit SimulationSettings(double confidence_level = 0.95,
                                std::size_t worst_scenario_count = 3);

    [[nodiscard]] double confidenceLevel() const noexcept;
    [[nodiscard]] std::size_t worstScenarioCount() const noexcept;

  private:
    double m_confidence_level;
    std::size_t m_worst_scenario_count;
};

class SimulationReport final
{
  public:
    SimulationReport(PortfolioValuation current_portfolio,
                     std::vector<ScenarioResult> scenario_results,
                     RiskMetrics risk_metrics,
                     std::chrono::nanoseconds runtime);

    [[nodiscard]] const PortfolioValuation& currentPortfolio() const noexcept;
    [[nodiscard]] std::span<const ScenarioResult>
    scenarioResults() const noexcept;
    [[nodiscard]] const RiskMetrics& riskMetrics() const noexcept;
    [[nodiscard]] std::chrono::nanoseconds runtime() const noexcept;
    [[nodiscard]] double scenariosPerSecond() const noexcept;

  private:
    PortfolioValuation m_current_portfolio;
    std::vector<ScenarioResult> m_scenario_results;
    RiskMetrics m_risk_metrics;
    std::chrono::nanoseconds m_runtime;
};

class SimulationEngine final
{
  public:
    [[nodiscard]] SimulationReport
    run(const Portfolio& portfolio, const MarketState& market_state,
        std::span<const Scenario *const> scenarios,
        const SimulationSettings& settings = SimulationSettings{}) const;
};

} // namespace htpste

#endif // SIMULATION_ENGINE_H
