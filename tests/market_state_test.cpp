#include "htpste/domain/market_state.hpp"

#include <gtest/gtest.h>

#include <limits>
#include <stdexcept>

namespace htpste {

TEST(MarketStateTest, ReturnsPricesAndParametersWithoutMutatingState) {
    const MarketState state{
        PriceMap{{"AAPL", 210.25}, {"MSFT", 430.0}},
        ModelParameters{ParameterMap{{"risk_free_rate", 0.03}}}
    };

    EXPECT_EQ(state.priceFor("AAPL"), 210.25);
    EXPECT_EQ(state.parameter("risk_free_rate"), 0.03);
    EXPECT_EQ(state.priceFor("UNKNOWN"), std::nullopt);
    EXPECT_EQ(state.parameter("UNKNOWN"), std::nullopt);
}

TEST(MarketStateTest, AllowsZeroButRejectsInvalidPrices) {
    EXPECT_NO_THROW((MarketState{PriceMap{{"BANKRUPT", 0.0}}}));
    EXPECT_THROW((MarketState{PriceMap{{"AAPL", -1.0}}}), std::invalid_argument);
    EXPECT_THROW(
        (MarketState{PriceMap{{"AAPL", std::numeric_limits<double>::quiet_NaN()}}}),
        std::invalid_argument
    );
    EXPECT_THROW((MarketState{PriceMap{{" ", 10.0}}}), std::invalid_argument);
}

TEST(MarketStateTest, RejectsInvalidModelParameters) {
    EXPECT_THROW(
        (MarketState{
            PriceMap{},
            ModelParameters{ParameterMap{{"volatility", std::numeric_limits<double>::infinity()}}}
        }),
        std::invalid_argument
    );
    EXPECT_THROW(
        (MarketState{PriceMap{}, ModelParameters{ParameterMap{{"", 1.0}}}}),
        std::invalid_argument
    );
}

}  // namespace htpste
