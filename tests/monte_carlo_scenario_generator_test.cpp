#include "htpste/scenarios/monte_carlo_scenario_generator.hpp"

#include <gtest/gtest.h>

#include <Eigen/Core>

#include <cstddef>
#include <stdexcept>
#include <string_view>
#include <vector>

namespace htpste
{
namespace
{
[[nodiscard]] MarketModel makeMarketModel()
{
    Eigen::VectorXd prices(2);
    prices << 100.0, 200.0;
    Eigen::VectorXd means(2);
    means << 0.001, -0.002;
    Eigen::MatrixXd covariance(2, 2);
    covariance << 0.04, 0.01, 0.01, 0.09;
    return MarketModel{{"AAPL", "MSFT"}, std::move(prices), std::move(means),
                       std::move(covariance)};
}

void expectSameScenarios(const std::vector<MonteCarloScenario>& first,
                         const std::vector<MonteCarloScenario>& second)
{
    ASSERT_EQ(first.size(), second.size());
    for (std::size_t scenario_id = 0; scenario_id < first.size(); ++scenario_id)
    {
        EXPECT_EQ(first[scenario_id].scenarioId(), scenario_id);
        EXPECT_EQ(second[scenario_id].scenarioId(), scenario_id);
        for (const std::string_view symbol : {"AAPL", "MSFT"})
        {
            ASSERT_TRUE(first[scenario_id].priceShockFor(symbol).has_value());
            ASSERT_TRUE(second[scenario_id].priceShockFor(symbol).has_value());
            EXPECT_DOUBLE_EQ(*first[scenario_id].priceShockFor(symbol),
                             *second[scenario_id].priceShockFor(symbol));
        }
    }
}

} // namespace

TEST(MonteCarloScenarioGeneratorTest, RepeatsExactlyForTheSameSeed)
{
    const MonteCarloScenarioGenerator generator{makeMarketModel()};

    const auto first = generator.generate(16, 42, 1);
    const auto second = generator.generate(16, 42, 1);

    expectSameScenarios(first, second);
}

TEST(MonteCarloScenarioGeneratorTest, IsIndependentOfThreadCount)
{
    const MonteCarloScenarioGenerator generator{makeMarketModel()};

    const auto sequential = generator.generate(101, 987654321, 1);
    const auto parallel = generator.generate(101, 987654321, 7);

    expectSameScenarios(sequential, parallel);
}

TEST(MonteCarloScenarioGeneratorTest, DifferentSeedsProduceDifferentScenarios)
{
    const MonteCarloScenarioGenerator generator{makeMarketModel()};

    const auto first = generator.generate(1, 42);
    const auto second = generator.generate(1, 43);

    EXPECT_NE(first[0].priceShockFor("AAPL"),
              second[0].priceShockFor("AAPL"));
}

TEST(MonteCarloScenarioGeneratorTest, ConvertsLogReturnsToRelativePriceShocks)
{
    Eigen::VectorXd prices(1);
    prices << 100.0;
    Eigen::VectorXd means(1);
    means << 0.0;
    Eigen::MatrixXd covariance(1, 1);
    covariance << 0.04;
    const MonteCarloScenarioGenerator generator{MarketModel{
        {"AAPL"}, std::move(prices), std::move(means), std::move(covariance)}};

    const MonteCarloScenario scenario = generator.generate(1, 42).front();
    const double shock = *scenario.priceShockFor("AAPL");

    EXPECT_GT(shock, -1.0);
    EXPECT_DOUBLE_EQ(*scenario.stressedPriceFor("AAPL", 100.0),
                     100.0 * (1.0 + shock));
}

TEST(MonteCarloScenarioGeneratorTest, ValidatesThreadCountAndEmptyRequest)
{
    const MonteCarloScenarioGenerator generator{makeMarketModel()};

    EXPECT_THROW(static_cast<void>(generator.generate(1, 42, 0)),
                 std::invalid_argument);
    EXPECT_TRUE(generator.generate(0, 42, 8).empty());
}

} // namespace htpste
