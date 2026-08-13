#include "htpste/market/market_model_loader.hpp"

#include <gtest/gtest.h>

#include <sstream>
#include <stdexcept>

namespace htpste
{
namespace
{
constexpr auto valid_market_model = R"json(
{
    "name": ["AAPL", "MSFT"],
    "prices": [100.0, 200.0],
    "mean_log_returns": [0.001, -0.002],
    "covariance": [[0.04, 0.01], [0.01, 0.09]]
}
)json";

} // namespace

TEST(MarketModelLoaderTest, LoadsNamedAssetsAndNumericalModelData)
{
    std::istringstream input{valid_market_model};

    const MarketModel model = MarketModelLoader{}.load(input);

    ASSERT_EQ(model.assetCount(), 2U);
    EXPECT_EQ(model.symbols()[0], "AAPL");
    EXPECT_EQ(model.symbols()[1], "MSFT");
    EXPECT_DOUBLE_EQ(model.prices()[0], 100.0);
    EXPECT_DOUBLE_EQ(model.meanLogReturns()[1], -0.002);
    EXPECT_DOUBLE_EQ(model.covariance()(0, 1), 0.01);
    EXPECT_EQ(model.indexOf("MSFT"), 1U);
    EXPECT_EQ(model.indexOf("UNKNOWN"), std::nullopt);
}

TEST(MarketModelLoaderTest, AcceptsSymbolsKeyProducedByPythonGenerator)
{
    std::istringstream input{R"json(
        {
            "symbols": ["AAPL"],
            "prices": [100.0],
            "mean_log_returns": [0.001],
            "covariance": [[0.04]]
        }
    )json"};

    const MarketModel model = MarketModelLoader{}.load(input);

    ASSERT_EQ(model.assetCount(), 1U);
    EXPECT_EQ(model.symbols()[0], "AAPL");
}

TEST(MarketModelLoaderTest, RejectsMalformedJson)
{
    std::istringstream input{"{not valid json}"};

    EXPECT_THROW(static_cast<void>(MarketModelLoader{}.load(input)),
                 std::invalid_argument);
}

TEST(MarketModelLoaderTest, RejectsMismatchedDimensions)
{
    std::istringstream input{R"json(
        {
            "name": ["AAPL", "MSFT"],
            "prices": [100.0],
            "mean_log_returns": [0.001, -0.002],
            "covariance": [[0.04, 0.01], [0.01, 0.09]]
        }
    )json"};

    EXPECT_THROW(static_cast<void>(MarketModelLoader{}.load(input)),
                 std::invalid_argument);
}

TEST(MarketModelLoaderTest, RejectsNonPositiveDefiniteCovariance)
{
    std::istringstream input{R"json(
        {
            "name": ["AAPL", "MSFT"],
            "prices": [100.0, 200.0],
            "mean_log_returns": [0.001, -0.002],
            "covariance": [[1.0, 2.0], [2.0, 1.0]]
        }
    )json"};

    EXPECT_THROW(static_cast<void>(MarketModelLoader{}.load(input)),
                 std::invalid_argument);
}

TEST(MarketModelLoaderTest, LoadsGeneratedPythonMarketModel)
{
    const MarketModel model = MarketModelLoader{}.loadFromFile(
        HTPSTE_SOURCE_DIR "/python/market_model.json");

    EXPECT_EQ(model.assetCount(), 15U);
    EXPECT_EQ(model.symbols()[0], "AAPL");
    EXPECT_GT(model.prices()[0], 0.0);
}

} // namespace htpste
