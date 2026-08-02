#include "htpste/domain/portfolio.hpp"

#include <gtest/gtest.h>

#include <vector>

namespace htpste {

TEST(PortfolioTest, StartsEmptyAndAcceptsPositions) {
    Portfolio portfolio;

    EXPECT_TRUE(portfolio.empty());
    portfolio.addPosition(Position{"AAPL", 10.0});

    ASSERT_EQ(portfolio.size(), 1U);
    EXPECT_EQ(portfolio.positions().front().symbol(), "AAPL");
}

TEST(PortfolioTest, PreservesPositionOrder) {
    const Portfolio portfolio{
        std::vector{Position{"AAPL", 10.0}, Position{"MSFT", -3.0}}
    };

    ASSERT_EQ(portfolio.positions().size(), 2U);
    EXPECT_EQ(portfolio.positions()[0].symbol(), "AAPL");
    EXPECT_EQ(portfolio.positions()[1].symbol(), "MSFT");
}

}  // namespace htpste

