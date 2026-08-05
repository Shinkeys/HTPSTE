#ifndef DETERMINISTIC_SCENARIOS_H
#define DETERMINISTIC_SCENARIOS_H

#include "htpste/scenarios/scenario.hpp"

#include <string_view>

namespace htpste
{
class CovidCrashScenario final : public Scenario
{
  public:
    CovidCrashScenario();
    [[nodiscard]] std::string_view name() const noexcept override;
};

class GlobalFinancialCrisisScenario final : public Scenario
{
  public:
    GlobalFinancialCrisisScenario();
    [[nodiscard]] std::string_view name() const noexcept override;
};

class DotComCrashScenario final : public Scenario
{
  public:
    DotComCrashScenario();
    [[nodiscard]] std::string_view name() const noexcept override;
};

class RateShockScenario final : public Scenario
{
  public:
    RateShockScenario();
    [[nodiscard]] std::string_view name() const noexcept override;
};

class OilCollapseScenario final : public Scenario
{
  public:
    OilCollapseScenario();
    [[nodiscard]] std::string_view name() const noexcept override;
};

class FlashCrashScenario final : public Scenario
{
  public:
    FlashCrashScenario();
    [[nodiscard]] std::string_view name() const noexcept override;
};

} // namespace htpste

#endif // DETERMINISTIC_SCENARIOS_H
