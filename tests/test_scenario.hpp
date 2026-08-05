#ifndef TEST_SCENARIO_H
#define TEST_SCENARIO_H

#include "htpste/scenarios/scenario.hpp"

#include <string>
#include <string_view>
#include <utility>

namespace htpste
{
class TestScenario final : public Scenario
{
  public:
    TestScenario(std::string name, PriceShockMap price_shocks)
        : Scenario(std::move(price_shocks)), m_name(std::move(name))
    {
    }

    [[nodiscard]] std::string_view name() const noexcept override
    {
        return m_name;
    }

  private:
    std::string m_name;
};

} // namespace htpste

#endif // TEST_SCENARIO_H
