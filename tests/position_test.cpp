#include "htpste/domain/position.hpp"

#include <gtest/gtest.h>

#include <limits>
#include <stdexcept>

namespace htpste {

TEST(PositionTest, RepresentsLongAndShortQuantitiesWithoutPrices) {
    const Position longPosition{"AAPL", 12.5};
    const Position shortPosition{"MSFT", -4.0};

    EXPECT_EQ(longPosition.symbol(), "AAPL");
    EXPECT_DOUBLE_EQ(longPosition.quantity(), 12.5);
    EXPECT_TRUE(longPosition.isLong());
    EXPECT_FALSE(longPosition.isShort());
    EXPECT_TRUE(shortPosition.isShort());
    EXPECT_FALSE(shortPosition.isLong());
}

TEST(PositionTest, RejectsInvalidInput) {
    EXPECT_THROW((Position{"", 1.0}), std::invalid_argument);
    EXPECT_THROW((Position{"   ", 1.0}), std::invalid_argument);
    EXPECT_THROW((Position{"AAPL", 0.0}), std::invalid_argument);
    EXPECT_THROW(
        (Position{"AAPL", std::numeric_limits<double>::infinity()}),
        std::invalid_argument
    );
}

}  // namespace htpste

