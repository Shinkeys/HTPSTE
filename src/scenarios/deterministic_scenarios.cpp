#include "htpste/scenarios/deterministic_scenarios.hpp"

#include "htpste/domain/instrument_symbols.hpp"

#include <initializer_list>
#include <string>
#include <string_view>
#include <utility>

namespace htpste
{
namespace
{
// TODO: replace it later with automatical scenario generation model
using Shock = std::pair<std::string_view, double>;

[[nodiscard]] PriceShockMap
makePriceShocks(const std::initializer_list<Shock> shocks)
{
    PriceShockMap result;
    result.reserve(shocks.size());
    for (const auto& [symbol, shock] : shocks)
    {
        result.emplace(std::string{symbol}, shock);
    }
    return result;
}

} // namespace

CovidCrashScenario::CovidCrashScenario()
    : Scenario(makePriceShocks({
          {instrument_symbols::gold, 0.08},
          {instrument_symbols::nvidia, -0.30},
          {instrument_symbols::apple, -0.28},
          {instrument_symbols::alphabet, -0.25},
          {instrument_symbols::microsoft, -0.22},
          {instrument_symbols::silver, -0.20},
          {instrument_symbols::amazon, -0.12},
          {instrument_symbols::tsmc, -0.25},
          {instrument_symbols::broadcom, -0.32},
          {instrument_symbols::saudi_aramco, -0.35},
          {instrument_symbols::spacex, -0.30},
          {instrument_symbols::meta, -0.28},
          {instrument_symbols::bitcoin, -0.45},
          {instrument_symbols::tesla, -0.40},
          {instrument_symbols::samsung, -0.24},
          {instrument_symbols::berkshire_hathaway, -0.30},
          {instrument_symbols::eli_lilly, -0.18},
          {instrument_symbols::vanguard_sp_500_etf, -0.30},
          {instrument_symbols::jpmorgan_chase, -0.38},
          {instrument_symbols::micron_technology, -0.35},
          {instrument_symbols::sp_500, -0.30},
      }))
{
}

std::string_view CovidCrashScenario::name() const noexcept
{
    return "COVID crash";
}

GlobalFinancialCrisisScenario::GlobalFinancialCrisisScenario()
    : Scenario(makePriceShocks({
          {instrument_symbols::gold, 0.05},
          {instrument_symbols::nvidia, -0.55},
          {instrument_symbols::apple, -0.50},
          {instrument_symbols::alphabet, -0.45},
          {instrument_symbols::microsoft, -0.40},
          {instrument_symbols::silver, -0.30},
          {instrument_symbols::amazon, -0.55},
          {instrument_symbols::tsmc, -0.50},
          {instrument_symbols::broadcom, -0.55},
          {instrument_symbols::saudi_aramco, -0.45},
          {instrument_symbols::spacex, -0.50},
          {instrument_symbols::meta, -0.50},
          {instrument_symbols::bitcoin, -0.60},
          {instrument_symbols::tesla, -0.55},
          {instrument_symbols::samsung, -0.45},
          {instrument_symbols::berkshire_hathaway, -0.35},
          {instrument_symbols::eli_lilly, -0.25},
          {instrument_symbols::vanguard_sp_500_etf, -0.45},
          {instrument_symbols::jpmorgan_chase, -0.60},
          {instrument_symbols::micron_technology, -0.60},
          {instrument_symbols::sp_500, -0.45},
      }))
{
}

std::string_view GlobalFinancialCrisisScenario::name() const noexcept
{
    return "Global Financial Crisis";
}

DotComCrashScenario::DotComCrashScenario()
    : Scenario(makePriceShocks({
          {instrument_symbols::gold, 0.02},
          {instrument_symbols::nvidia, -0.70},
          {instrument_symbols::apple, -0.65},
          {instrument_symbols::alphabet, -0.70},
          {instrument_symbols::microsoft, -0.60},
          {instrument_symbols::silver, -0.10},
          {instrument_symbols::amazon, -0.85},
          {instrument_symbols::tsmc, -0.60},
          {instrument_symbols::broadcom, -0.65},
          {instrument_symbols::saudi_aramco, -0.15},
          {instrument_symbols::spacex, -0.65},
          {instrument_symbols::meta, -0.70},
          {instrument_symbols::bitcoin, -0.65},
          {instrument_symbols::tesla, -0.60},
          {instrument_symbols::samsung, -0.50},
          {instrument_symbols::berkshire_hathaway, -0.10},
          {instrument_symbols::eli_lilly, -0.08},
          {instrument_symbols::vanguard_sp_500_etf, -0.40},
          {instrument_symbols::jpmorgan_chase, -0.20},
          {instrument_symbols::micron_technology, -0.70},
          {instrument_symbols::sp_500, -0.40},
      }))
{
}

std::string_view DotComCrashScenario::name() const noexcept
{
    return "Dot-com crash";
}

RateShockScenario::RateShockScenario()
    : Scenario(makePriceShocks({
                   {instrument_symbols::gold, -0.12},
                   {instrument_symbols::nvidia, -0.25},
                   {instrument_symbols::apple, -0.18},
                   {instrument_symbols::alphabet, -0.20},
                   {instrument_symbols::microsoft, -0.15},
                   {instrument_symbols::silver, -0.15},
                   {instrument_symbols::amazon, -0.22},
                   {instrument_symbols::tsmc, -0.18},
                   {instrument_symbols::broadcom, -0.20},
                   {instrument_symbols::saudi_aramco, -0.08},
                   {instrument_symbols::spacex, -0.30},
                   {instrument_symbols::meta, -0.20},
                   {instrument_symbols::bitcoin, -0.30},
                   {instrument_symbols::tesla, -0.28},
                   {instrument_symbols::samsung, -0.15},
                   {instrument_symbols::berkshire_hathaway, -0.10},
                   {instrument_symbols::eli_lilly, -0.12},
                   {instrument_symbols::vanguard_sp_500_etf, -0.15},
                   {instrument_symbols::jpmorgan_chase, 0.05},
                   {instrument_symbols::micron_technology, -0.22},
                   {instrument_symbols::sp_500, -0.15},
               }),
               ParameterShiftMap{{"risk_free_rate", 0.03}})
{
}

std::string_view RateShockScenario::name() const noexcept
{
    return "Rate shock (+300 bps)";
}

OilCollapseScenario::OilCollapseScenario()
    : Scenario(makePriceShocks({
          {instrument_symbols::gold, 0.05},
          {instrument_symbols::nvidia, -0.08},
          {instrument_symbols::apple, -0.10},
          {instrument_symbols::alphabet, -0.08},
          {instrument_symbols::microsoft, -0.07},
          {instrument_symbols::silver, -0.12},
          {instrument_symbols::amazon, -0.06},
          {instrument_symbols::tsmc, -0.10},
          {instrument_symbols::broadcom, -0.10},
          {instrument_symbols::saudi_aramco, -0.45},
          {instrument_symbols::spacex, -0.12},
          {instrument_symbols::meta, -0.08},
          {instrument_symbols::bitcoin, -0.15},
          {instrument_symbols::tesla, -0.18},
          {instrument_symbols::samsung, -0.12},
          {instrument_symbols::berkshire_hathaway, -0.12},
          {instrument_symbols::eli_lilly, -0.04},
          {instrument_symbols::vanguard_sp_500_etf, -0.12},
          {instrument_symbols::jpmorgan_chase, -0.18},
          {instrument_symbols::micron_technology, -0.14},
          {instrument_symbols::sp_500, -0.12},
      }))
{
}

std::string_view OilCollapseScenario::name() const noexcept
{
    return "Oil collapse";
}

FlashCrashScenario::FlashCrashScenario()
    : Scenario(makePriceShocks({
          {instrument_symbols::gold, -0.02},
          {instrument_symbols::nvidia, -0.18},
          {instrument_symbols::apple, -0.12},
          {instrument_symbols::alphabet, -0.12},
          {instrument_symbols::microsoft, -0.10},
          {instrument_symbols::silver, -0.07},
          {instrument_symbols::amazon, -0.14},
          {instrument_symbols::tsmc, -0.15},
          {instrument_symbols::broadcom, -0.16},
          {instrument_symbols::saudi_aramco, -0.08},
          {instrument_symbols::spacex, -0.15},
          {instrument_symbols::meta, -0.14},
          {instrument_symbols::bitcoin, -0.25},
          {instrument_symbols::tesla, -0.22},
          {instrument_symbols::samsung, -0.12},
          {instrument_symbols::berkshire_hathaway, -0.09},
          {instrument_symbols::eli_lilly, -0.08},
          {instrument_symbols::vanguard_sp_500_etf, -0.10},
          {instrument_symbols::jpmorgan_chase, -0.15},
          {instrument_symbols::micron_technology, -0.18},
          {instrument_symbols::sp_500, -0.10},
      }))
{
}

std::string_view FlashCrashScenario::name() const noexcept
{
    return "Flash crash";
}

} // namespace htpste
