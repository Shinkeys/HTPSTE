#include "htpste/risk/risk_metrics.hpp"

#include <algorithm>
#include <cmath>
#include <numeric>
#include <stdexcept>
#include <utility>

namespace htpste
{
WorstScenario::WorstScenario(const std::uint64_t scenario_id,
                             std::string scenario_name, const double pnl)
    : m_scenario_id(scenario_id), m_scenario_name(std::move(scenario_name)),
      m_pnl(pnl)
{
}

std::uint64_t WorstScenario::scenarioId() const noexcept
{
    return m_scenario_id;
}

const std::string& WorstScenario::scenarioName() const noexcept
{
    return m_scenario_name;
}

double WorstScenario::pnl() const noexcept
{
    return m_pnl;
}

double WorstScenario::loss() const noexcept
{
    return -m_pnl;
}

RiskMetrics::RiskMetrics(const double confidence_level,
                         const double value_at_risk,
                         const double conditional_value_at_risk,
                         std::vector<WorstScenario> worst_scenarios)
    : m_confidence_level(confidence_level), m_value_at_risk(value_at_risk),
      m_conditional_value_at_risk(conditional_value_at_risk),
      m_worst_scenarios(std::move(worst_scenarios))
{
}

double RiskMetrics::confidenceLevel() const noexcept
{
    return m_confidence_level;
}

double RiskMetrics::valueAtRisk() const noexcept
{
    return m_value_at_risk;
}

double RiskMetrics::conditionalValueAtRisk() const noexcept
{
    return m_conditional_value_at_risk;
}

std::span<const WorstScenario> RiskMetrics::worstScenarios() const noexcept
{
    return m_worst_scenarios;
}

RiskMetrics RiskCalculator::calculate(
    const std::span<const ScenarioResult> scenario_results,
    const double confidence_level, const std::size_t worst_scenario_count) const
{
    if (scenario_results.empty())
    {
        throw std::invalid_argument(
            "risk metrics require at least one scenario result");
    }
    if (!std::isfinite(confidence_level) || confidence_level <= 0.0 ||
        confidence_level >= 1.0)
    {
        throw std::invalid_argument(
            "confidence level must be finite and between 0 and 1");
    }

    std::vector<double> sorted_losses;
    sorted_losses.reserve(scenario_results.size());
    for (const auto& result : scenario_results)
    {
        if (!std::isfinite(result.pnl()))
        {
            throw std::invalid_argument("scenario P&L must be finite");
        }
        sorted_losses.push_back(result.loss());
    }
    std::ranges::sort(sorted_losses);

    const auto rank = static_cast<std::size_t>(std::ceil(
        confidence_level * static_cast<double>(sorted_losses.size())));
    const std::size_t var_index = std::max<std::size_t>(rank, 1U) - 1U;
    const double value_at_risk = sorted_losses[var_index];
    const double tail_sum = std::accumulate(
        sorted_losses.begin() + static_cast<std::ptrdiff_t>(var_index),
        sorted_losses.end(), 0.0);
    const double conditional_value_at_risk =
        tail_sum / static_cast<double>(sorted_losses.size() - var_index);

    std::vector<const ScenarioResult *> ordered_results;
    ordered_results.reserve(scenario_results.size());
    for (const auto& result : scenario_results)
    {
        ordered_results.push_back(&result);
    }
    std::ranges::sort(ordered_results,
                      [](const auto *left, const auto *right)
                      {
                          if (left->pnl() != right->pnl())
                          {
                              return left->pnl() < right->pnl();
                          }
                          return left->scenarioId() < right->scenarioId();
                      });

    const std::size_t retained_count =
        std::min(worst_scenario_count, ordered_results.size());
    std::vector<WorstScenario> worst_scenarios;
    worst_scenarios.reserve(retained_count);
    for (std::size_t index = 0; index < retained_count; ++index)
    {
        const auto& result = *ordered_results[index];
        worst_scenarios.emplace_back(result.scenarioId(), result.scenarioName(),
                                     result.pnl());
    }

    return RiskMetrics{confidence_level, value_at_risk,
                       conditional_value_at_risk, std::move(worst_scenarios)};
}

} // namespace htpste
