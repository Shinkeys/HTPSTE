#ifndef HTPSTE_MONTE_CARLO_SCENARIO_HPP
#define HTPSTE_MONTE_CARLO_SCENARIO_HPP

#include "htpste/scenarios/scenario.hpp"

#include <cstddef>
#include <string>
#include <string_view>

namespace htpste
{
class MonteCarloScenario final : public Scenario
{
  public:
    MonteCarloScenario(std::size_t scenario_id, PriceShockMap price_shocks);

    [[nodiscard]] std::size_t scenarioId() const noexcept;
    [[nodiscard]] std::string_view name() const noexcept override;

  private:
    std::size_t m_scenario_id;
    std::string m_name;
};

} // namespace htpste

#endif // HTPSTE_MONTE_CARLO_SCENARIO_HPP
