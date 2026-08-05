#include "htpste/risk/risk_metrics.hpp"

#include <gtest/gtest.h>

#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

namespace htpste
{
namespace
{
[[nodiscard]] ScenarioResult makeResult(const std::uint64_t scenario_id,
                                        const double pnl)
{
    std::vector<PositionRepricing> positions;
    positions.emplace_back("TEST", 1.0, 1000.0, pnl / 1000.0, 1000.0 + pnl,
                           1000.0, 1000.0 + pnl, pnl);
    return ScenarioResult{scenario_id,
                          "Scenario " + std::to_string(scenario_id), 1000.0,
                          std::move(positions)};
}

} // namespace

TEST(RiskMetricsTest, CalculatesNearestRankVarCvarAndWorstScenarios)
{
    const std::vector<ScenarioResult> results{
        makeResult(0, 100.0), makeResult(1, -10.0),  makeResult(2, -20.0),
        makeResult(3, -30.0), makeResult(4, -100.0),
    };

    const RiskMetrics metrics = RiskCalculator{}.calculate(results, 0.80, 3);

    EXPECT_DOUBLE_EQ(metrics.valueAtRisk(), 30.0);
    EXPECT_DOUBLE_EQ(metrics.conditionalValueAtRisk(), 65.0);
    ASSERT_EQ(metrics.worstScenarios().size(), 3U);
    EXPECT_EQ(metrics.worstScenarios()[0].scenarioId(), 4U);
    EXPECT_EQ(metrics.worstScenarios()[1].scenarioId(), 3U);
    EXPECT_EQ(metrics.worstScenarios()[2].scenarioId(), 2U);
}

TEST(RiskMetricsTest, OrdersEqualLossesByScenarioId)
{
    const std::vector<ScenarioResult> results{
        makeResult(9, -50.0),
        makeResult(2, -50.0),
    };

    const RiskMetrics metrics = RiskCalculator{}.calculate(results, 0.50, 10);

    ASSERT_EQ(metrics.worstScenarios().size(), 2U);
    EXPECT_EQ(metrics.worstScenarios()[0].scenarioId(), 2U);
    EXPECT_EQ(metrics.worstScenarios()[1].scenarioId(), 9U);
}

TEST(RiskMetricsTest, RejectsEmptyResultsAndInvalidConfidence)
{
    const std::vector<ScenarioResult> empty;
    const std::vector<ScenarioResult> one_result{makeResult(0, -10.0)};

    EXPECT_THROW(static_cast<void>(RiskCalculator{}.calculate(empty, 0.95, 1)),
                 std::invalid_argument);
    EXPECT_THROW(
        static_cast<void>(RiskCalculator{}.calculate(one_result, 1.0, 1)),
        std::invalid_argument);
    EXPECT_THROW(static_cast<void>(RiskCalculator{}.calculate(
                     one_result, std::numeric_limits<double>::quiet_NaN(), 1)),
                 std::invalid_argument);
}

} // namespace htpste
