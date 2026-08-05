#ifndef RISK_METRICS_H
#define RISK_METRICS_H

#include "htpste/pricing/scenario_repricing.hpp"

#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <vector>

namespace htpste
{
class WorstScenario final
{
  public:
    WorstScenario(std::uint64_t scenario_id, std::string scenario_name,
                  double pnl);

    [[nodiscard]] std::uint64_t scenarioId() const noexcept;
    [[nodiscard]] const std::string& scenarioName() const noexcept;
    [[nodiscard]] double pnl() const noexcept;
    [[nodiscard]] double loss() const noexcept;

  private:
    std::uint64_t m_scenario_id;
    std::string m_scenario_name;
    double m_pnl;
};

class RiskMetrics final
{
  public:
    RiskMetrics(double confidence_level, double value_at_risk,
                double conditional_value_at_risk,
                std::vector<WorstScenario> worst_scenarios);

    [[nodiscard]] double confidenceLevel() const noexcept;
    [[nodiscard]] double valueAtRisk() const noexcept;
    [[nodiscard]] double conditionalValueAtRisk() const noexcept;
    [[nodiscard]] std::span<const WorstScenario>
    worstScenarios() const noexcept;

  private:
    double m_confidence_level;
    double m_value_at_risk;
    double m_conditional_value_at_risk;
    std::vector<WorstScenario> m_worst_scenarios;
};

class RiskCalculator final
{
  public:
    // Uses the nearest-rank empirical quantile. CVaR is the mean of losses
    // from the VaR observation through the worst loss, inclusive.
    [[nodiscard]] RiskMetrics
    calculate(std::span<const ScenarioResult> scenario_results,
              double confidence_level, std::size_t worst_scenario_count) const;
};

} // namespace htpste

#endif // RISK_METRICS_H
