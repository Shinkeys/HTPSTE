#ifndef SIMULATION_VALIDATOR_H
#define SIMULATION_VALIDATOR_H

#include "htpste/domain/market_state.hpp"
#include "htpste/domain/portfolio.hpp"
#include "htpste/scenarios/scenario.hpp"

#include <span>

namespace htpste
{
void validateSimulationInputs(const Portfolio& portfolio,
                              const MarketState& market_state,
                              std::span<const Scenario *const> scenarios);

} // namespace htpste

#endif // SIMULATION_VALIDATOR_H
