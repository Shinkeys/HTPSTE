#include "htpste/domain/instrument_symbols.hpp"
#include "htpste/scenarios/deterministic_scenarios.hpp"

#include <gtest/gtest.h>

#include <array>
#include <memory>
#include <stdexcept>

namespace htpste
{
namespace
{
[[nodiscard]] std::array<std::unique_ptr<Scenario>, 6> makeScenarios()
{
    return {
        std::make_unique<CovidCrashScenario>(),
        std::make_unique<GlobalFinancialCrisisScenario>(),
        std::make_unique<DotComCrashScenario>(),
        std::make_unique<RateShockScenario>(),
        std::make_unique<OilCollapseScenario>(),
        std::make_unique<FlashCrashScenario>(),
    };
}

} // namespace

TEST(ScenarioTest, EveryDeterministicScenarioCoversTheInstrumentUniverse)
{
    const auto scenarios = makeScenarios();

    for (const auto& scenario : scenarios)
    {
        EXPECT_FALSE(scenario->name().empty());
        EXPECT_EQ(scenario->priceShocks().size(),
                  instrument_symbols::all.size());
        for (const auto symbol : instrument_symbols::all)
        {
            EXPECT_TRUE(scenario->priceShockFor(symbol).has_value())
                << scenario->name() << " is missing " << symbol;
        }
    }
}

TEST(ScenarioTest, AppliesRelativePriceShock)
{
    const CovidCrashScenario scenario;

    EXPECT_DOUBLE_EQ(
        *scenario.stressedPriceFor(instrument_symbols::apple, 100.0), 72.0);
    EXPECT_EQ(scenario.stressedPriceFor("UNKNOWN", 100.0), std::nullopt);
    EXPECT_THROW(static_cast<void>(scenario.stressedPriceFor(
                     instrument_symbols::apple, -1.0)),
                 std::invalid_argument);
}

TEST(ScenarioTest, RateShockIncludesThreeHundredBasisPointParameterShift)
{
    const RateShockScenario scenario;

    EXPECT_EQ(scenario.parameterShiftFor("risk_free_rate"), 0.03);
    EXPECT_GT(*scenario.priceShockFor(instrument_symbols::jpmorgan_chase), 0.0);
    EXPECT_LT(*scenario.priceShockFor(instrument_symbols::spacex), 0.0);
}

TEST(ScenarioTest, OilCollapseHitsSaudiAramcoMoreThanTheBroadMarket)
{
    const OilCollapseScenario scenario;

    EXPECT_LT(*scenario.priceShockFor(instrument_symbols::saudi_aramco),
              *scenario.priceShockFor(instrument_symbols::sp_500));
}

} // namespace htpste
