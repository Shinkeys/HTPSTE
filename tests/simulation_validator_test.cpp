#include "htpste/validation/simulation_validator.hpp"

#include "test_scenario.hpp"

#include <gtest/gtest.h>

#include <array>
#include <stdexcept>
#include <vector>

namespace htpste
{
TEST(SimulationValidatorTest, AcceptsCompleteMarketAndScenarioInputs)
{
    const Portfolio portfolio{std::vector{Position{"AAPL", 10.0}}};
    const MarketState market_state{PriceMap{{"AAPL", 100.0}}};
    const TestScenario scenario{"Complete", PriceShockMap{{"AAPL", -0.20}}};
    const std::array<const Scenario *, 1> scenarios{&scenario};

    EXPECT_NO_THROW(
        validateSimulationInputs(portfolio, market_state, scenarios));
}

TEST(SimulationValidatorTest, RejectsEmptyPortfolioOrScenarioCollection)
{
    const Portfolio empty_portfolio;
    const Portfolio portfolio{std::vector{Position{"AAPL", 10.0}}};
    const MarketState market_state{PriceMap{{"AAPL", 100.0}}};
    const TestScenario scenario{"Complete", PriceShockMap{{"AAPL", -0.20}}};
    const std::array<const Scenario *, 1> scenarios{&scenario};
    const std::span<const Scenario *const> no_scenarios;

    EXPECT_THROW(
        validateSimulationInputs(empty_portfolio, market_state, scenarios),
        std::invalid_argument);
    EXPECT_THROW(
        validateSimulationInputs(portfolio, market_state, no_scenarios),
        std::invalid_argument);
}

TEST(SimulationValidatorTest, RejectsMissingPricesShocksAndNullScenarios)
{
    const Portfolio portfolio{std::vector{Position{"AAPL", 10.0}}};
    const MarketState missing_price{PriceMap{{"MSFT", 200.0}}};
    const MarketState market_state{PriceMap{{"AAPL", 100.0}}};
    const TestScenario incomplete{"Incomplete", PriceShockMap{{"MSFT", -0.20}}};
    const std::array<const Scenario *, 1> incomplete_scenarios{&incomplete};
    const std::array<const Scenario *, 1> null_scenarios{nullptr};

    EXPECT_THROW(validateSimulationInputs(portfolio, missing_price,
                                          incomplete_scenarios),
                 std::invalid_argument);
    EXPECT_THROW(
        validateSimulationInputs(portfolio, market_state, incomplete_scenarios),
        std::invalid_argument);
    EXPECT_THROW(
        validateSimulationInputs(portfolio, market_state, null_scenarios),
        std::invalid_argument);
}

TEST(SimulationValidatorTest, RejectsBlankScenarioName)
{
    const Portfolio portfolio{std::vector{Position{"AAPL", 10.0}}};
    const MarketState market_state{PriceMap{{"AAPL", 100.0}}};
    const TestScenario scenario{"   ", PriceShockMap{{"AAPL", -0.20}}};
    const std::array<const Scenario *, 1> scenarios{&scenario};

    EXPECT_THROW(validateSimulationInputs(portfolio, market_state, scenarios),
                 std::invalid_argument);
}

} // namespace htpste
