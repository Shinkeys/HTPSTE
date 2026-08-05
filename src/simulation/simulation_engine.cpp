#include "htpste/simulation/simulation_engine.hpp"

#include "htpste/validation/simulation_validator.hpp"

#include <cmath>
#include <stdexcept>
#include <utility>

namespace htpste
{
SimulationSettings::SimulationSettings(const double confidence_level,
                                       const std::size_t worst_scenario_count)
    : m_confidence_level(confidence_level),
      m_worst_scenario_count(worst_scenario_count)
{
    if (!std::isfinite(m_confidence_level) || m_confidence_level <= 0.0 ||
        m_confidence_level >= 1.0)
    {
        throw std::invalid_argument(
            "confidence level must be finite and between 0 and 1");
    }
}

double SimulationSettings::confidenceLevel() const noexcept
{
    return m_confidence_level;
}

std::size_t SimulationSettings::worstScenarioCount() const noexcept
{
    return m_worst_scenario_count;
}

SimulationReport::SimulationReport(PortfolioValuation current_portfolio,
                                   std::vector<ScenarioResult> scenario_results,
                                   RiskMetrics risk_metrics,
                                   const std::chrono::nanoseconds runtime)
    : m_current_portfolio(std::move(current_portfolio)),
      m_scenario_results(std::move(scenario_results)),
      m_risk_metrics(std::move(risk_metrics)), m_runtime(runtime)
{
}

const PortfolioValuation& SimulationReport::currentPortfolio() const noexcept
{
    return m_current_portfolio;
}

std::span<const ScenarioResult>
SimulationReport::scenarioResults() const noexcept
{
    return m_scenario_results;
}

const RiskMetrics& SimulationReport::riskMetrics() const noexcept
{
    return m_risk_metrics;
}

std::chrono::nanoseconds SimulationReport::runtime() const noexcept
{
    return m_runtime;
}

double SimulationReport::scenariosPerSecond() const noexcept
{
    const double elapsed_seconds =
        std::chrono::duration<double>{m_runtime}.count();
    if (elapsed_seconds == 0.0)
    {
        return 0.0;
    }
    return static_cast<double>(m_scenario_results.size()) / elapsed_seconds;
}

SimulationReport
SimulationEngine::run(const Portfolio& portfolio,
                      const MarketState& market_state,
                      const std::span<const Scenario *const> scenarios,
                      const SimulationSettings& settings) const
{
    validateSimulationInputs(portfolio, market_state, scenarios);

    const auto started_at = std::chrono::steady_clock::now();
    const PortfolioValuator valuator;
    PortfolioValuation current_portfolio =
        valuator.value(portfolio, market_state);

    const ScenarioRepricer repricer;
    std::vector<ScenarioResult> scenario_results;
    scenario_results.reserve(scenarios.size());
    for (std::size_t index = 0; index < scenarios.size(); ++index)
    {
        scenario_results.push_back(
            repricer.reprice(current_portfolio, *scenarios[index], index));
    }

    const RiskCalculator risk_calculator;
    RiskMetrics risk_metrics =
        risk_calculator.calculate(scenario_results, settings.confidenceLevel(),
                                  settings.worstScenarioCount());
    const auto runtime = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now() - started_at);

    return SimulationReport{std::move(current_portfolio),
                            std::move(scenario_results),
                            std::move(risk_metrics), runtime};
}

} // namespace htpste
