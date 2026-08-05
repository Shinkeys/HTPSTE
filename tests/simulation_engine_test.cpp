#include "htpste/simulation/simulation_engine.hpp"

#include "test_scenario.hpp"

#include <gtest/gtest.h>

#include <array>
#include <stdexcept>
#include <vector>

namespace htpste
{
TEST(SimulationEngineTest, RunsCompleteSequentialPipeline)
{
    const Portfolio portfolio{
        std::vector{Position{"AAPL", 10.0}, Position{"MSFT", -2.0}}};
    const MarketState market_state{PriceMap{{"AAPL", 100.0}, {"MSFT", 200.0}}};
    const TestScenario fall{"Fall",
                            PriceShockMap{{"AAPL", -0.20}, {"MSFT", -0.10}}};
    const TestScenario rally{"Rally",
                             PriceShockMap{{"AAPL", 0.10}, {"MSFT", 0.05}}};
    const std::array<const Scenario *, 2> scenarios{&fall, &rally};

    const SimulationReport report = SimulationEngine{}.run(
        portfolio, market_state, scenarios, SimulationSettings{0.50, 2});

    EXPECT_DOUBLE_EQ(report.currentPortfolio().totalValue(), 600.0);
    ASSERT_EQ(report.scenarioResults().size(), 2U);
    EXPECT_EQ(report.scenarioResults()[0].scenarioId(), 0U);
    EXPECT_DOUBLE_EQ(report.scenarioResults()[0].pnl(), -160.0);
    EXPECT_EQ(report.scenarioResults()[1].scenarioId(), 1U);
    // Relative shocks such as 0.10 are not exactly representable as binary
    // doubles.
    EXPECT_NEAR(report.scenarioResults()[1].pnl(), 80.0, 1e-9);
    EXPECT_NEAR(report.riskMetrics().valueAtRisk(), -80.0, 1e-9);
    EXPECT_NEAR(report.riskMetrics().conditionalValueAtRisk(), 40.0, 1e-9);
    ASSERT_EQ(report.riskMetrics().worstScenarios().size(), 2U);
    EXPECT_EQ(report.riskMetrics().worstScenarios()[0].scenarioName(), "Fall");
    EXPECT_GE(report.runtime().count(), 0);
}

TEST(SimulationEngineTest, RejectsInvalidSettingsBeforeRunning)
{
    EXPECT_THROW((SimulationSettings{0.0, 3}), std::invalid_argument);
    EXPECT_THROW((SimulationSettings{1.0, 3}), std::invalid_argument);
}

} // namespace htpste
