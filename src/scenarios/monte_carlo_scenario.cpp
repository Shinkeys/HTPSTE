#include "htpste/scenarios/monte_carlo_scenario.hpp"

#include <string>
#include <utility>

namespace htpste
{
MonteCarloScenario::MonteCarloScenario(const std::size_t scenario_id,
                                       PriceShockMap price_shocks)
    : Scenario(std::move(price_shocks)), m_scenario_id(scenario_id),
      m_name("Monte Carlo scenario " + std::to_string(scenario_id))
{
}

std::size_t MonteCarloScenario::scenarioId() const noexcept
{
    return m_scenario_id;
}

std::string_view MonteCarloScenario::name() const noexcept
{
    return m_name;
}

} // namespace htpste
