#include "htpste/pricing/portfolio_valuation.hpp"

#include <gtest/gtest.h>

#include <limits>
#include <stdexcept>
#include <vector>

namespace htpste
{
TEST(PortfolioValuationTest, ValuesEveryPositionAndPortfolioExposure)
{
    const Portfolio portfolio{
        std::vector{Position{"AAPL", 10.0}, Position{"MSFT", -2.0}}};
    const MarketState market_state{PriceMap{{"AAPL", 100.0}, {"MSFT", 200.0}}};

    const PortfolioValuation valuation =
        PortfolioValuator{}.value(portfolio, market_state);

    ASSERT_EQ(valuation.positions().size(), 2U);
    EXPECT_EQ(valuation.positions()[0].position().symbol(), "AAPL");
    EXPECT_DOUBLE_EQ(valuation.positions()[0].position().quantity(), 10.0);
    EXPECT_DOUBLE_EQ(valuation.positions()[0].marketValue(), 1000.0);
    EXPECT_DOUBLE_EQ(valuation.positions()[1].marketValue(), -400.0);
    EXPECT_DOUBLE_EQ(valuation.totalValue(), 600.0);
    EXPECT_DOUBLE_EQ(valuation.longExposure(), 1000.0);
    EXPECT_DOUBLE_EQ(valuation.shortExposure(), 400.0);
    EXPECT_DOUBLE_EQ(valuation.grossExposure(), 1400.0);
}

TEST(PortfolioValuationTest, RejectsMissingMarketPrice)
{
    const Portfolio portfolio{std::vector{Position{"AAPL", 10.0}}};
    const MarketState market_state{PriceMap{{"MSFT", 200.0}}};

    EXPECT_THROW(
        static_cast<void>(PortfolioValuator{}.value(portfolio, market_state)),
        std::invalid_argument);
}

TEST(PortfolioValuationTest, RejectsOverflowingPositionValue)
{
    const Portfolio portfolio{
        std::vector{Position{"AAPL", std::numeric_limits<double>::max()}}};
    const MarketState market_state{
        PriceMap{{"AAPL", std::numeric_limits<double>::max()}}};

    EXPECT_THROW(
        static_cast<void>(PortfolioValuator{}.value(portfolio, market_state)),
        std::overflow_error);
}

} // namespace htpste
