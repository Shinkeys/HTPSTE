#include "htpste/validation/simulation_validator.hpp"

#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <string>
#include <string_view>

namespace htpste
{
namespace
{
[[nodiscard]] bool isBlank(const std::string_view value)
{
    return value.empty() ||
           std::ranges::all_of(value, [](const unsigned char character)
                               { return std::isspace(character) != 0; });
}

} // namespace

void validateSimulationInputs(const Portfolio& portfolio,
                              const MarketState& market_state,
                              const std::span<const Scenario *const> scenarios)
{
    if (portfolio.empty())
    {
        throw std::invalid_argument(
            "simulation portfolio must contain at least one position");
    }
    if (scenarios.empty())
    {
        throw std::invalid_argument(
            "simulation requires at least one scenario");
    }

    for (const auto& position : portfolio.positions())
    {
        if (!market_state.priceFor(position.symbol()).has_value())
        {
            throw std::invalid_argument(
                "missing current price for position symbol: " +
                position.symbol());
        }
    }

    for (const Scenario *scenario : scenarios)
    {
        if (scenario == nullptr)
        {
            throw std::invalid_argument(
                "scenario list must not contain null pointers");
        }
        if (isBlank(scenario->name()))
        {
            throw std::invalid_argument("scenario name must not be blank");
        }

        for (const auto& position : portfolio.positions())
        {
            if (!scenario->priceShockFor(position.symbol()).has_value())
            {
                throw std::invalid_argument(
                    "scenario '" + std::string{scenario->name()} +
                    "' is missing a price shock for position symbol: " +
                    position.symbol());
            }
        }
    }
}

} // namespace htpste
