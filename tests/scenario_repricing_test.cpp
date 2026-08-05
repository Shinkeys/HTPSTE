#include "htpste/pricing/scenario_repricing.hpp"

#include "test_scenario.hpp"

#include <gtest/gtest.h>

#include <stdexcept>
#include <vector>

namespace htpste
{
TEST(ScenarioRepricingTest, ReportsPriceAndPnlImpactForLongAndShortPositions)
{
    const Portfolio portfolio{
        std::vector{Position{"AAPL", 10.0}, Position{"MSFT", -2.0}}};
    const MarketState market_state{PriceMap{{"AAPL", 100.0}, {"MSFT", 200.0}}};
    const PortfolioValuation current =
        PortfolioValuator{}.value(portfolio, market_state);
    const TestScenario scenario{
        "Market fall", PriceShockMap{{"AAPL", -0.20}, {"MSFT", -0.10}}};

    const ScenarioResult result =
        ScenarioRepricer{}.reprice(current, scenario, 42);

    EXPECT_EQ(result.scenarioId(), 42U);
    EXPECT_EQ(result.scenarioName(), "Market fall");
    EXPECT_DOUBLE_EQ(result.currentPortfolioValue(), 600.0);
    EXPECT_DOUBLE_EQ(result.stressedPortfolioValue(), 440.0);
    EXPECT_DOUBLE_EQ(result.pnl(), -160.0);
    EXPECT_DOUBLE_EQ(result.loss(), 160.0);

    ASSERT_EQ(result.positions().size(), 2U);
    EXPECT_DOUBLE_EQ(result.positions()[0].stressedPrice(), 80.0);
    EXPECT_DOUBLE_EQ(result.positions()[0].pnl(), -200.0);
    EXPECT_DOUBLE_EQ(result.positions()[1].stressedPrice(), 180.0);
    EXPECT_DOUBLE_EQ(result.positions()[1].pnl(), 40.0);
}

TEST(ScenarioRepricingTest, RejectsScenarioMissingPositionShock)
{
    const Portfolio portfolio{
        std::vector{Position{"AAPL", 10.0}, Position{"MSFT", -2.0}}};
    const MarketState market_state{PriceMap{{"AAPL", 100.0}, {"MSFT", 200.0}}};
    const PortfolioValuation current =
        PortfolioValuator{}.value(portfolio, market_state);
    const TestScenario incomplete{"Incomplete", PriceShockMap{{"AAPL", -0.20}}};

    EXPECT_THROW(
        static_cast<void>(ScenarioRepricer{}.reprice(current, incomplete, 0)),
        std::invalid_argument);
}

} // namespace htpste
