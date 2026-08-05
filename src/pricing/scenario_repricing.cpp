#include "htpste/pricing/scenario_repricing.hpp"

#include <cmath>
#include <stdexcept>
#include <utility>

namespace htpste
{
PositionRepricing::PositionRepricing(
    std::string symbol, const double quantity, const double current_price,
    const double relative_shock, const double stressed_price,
    const double current_value, const double stressed_value, const double pnl)
    : m_symbol(std::move(symbol)), m_quantity(quantity),
      m_current_price(current_price), m_relative_shock(relative_shock),
      m_stressed_price(stressed_price), m_current_value(current_value),
      m_stressed_value(stressed_value), m_pnl(pnl)
{
}

const std::string& PositionRepricing::symbol() const noexcept
{
    return m_symbol;
}

double PositionRepricing::quantity() const noexcept
{
    return m_quantity;
}

double PositionRepricing::currentPrice() const noexcept
{
    return m_current_price;
}

double PositionRepricing::relativeShock() const noexcept
{
    return m_relative_shock;
}

double PositionRepricing::stressedPrice() const noexcept
{
    return m_stressed_price;
}

double PositionRepricing::currentValue() const noexcept
{
    return m_current_value;
}

double PositionRepricing::stressedValue() const noexcept
{
    return m_stressed_value;
}

double PositionRepricing::pnl() const noexcept
{
    return m_pnl;
}

ScenarioResult::ScenarioResult(const std::uint64_t scenario_id,
                               std::string scenario_name,
                               const double current_portfolio_value,
                               std::vector<PositionRepricing> positions)
    : m_scenario_id(scenario_id), m_scenario_name(std::move(scenario_name)),
      m_current_portfolio_value(current_portfolio_value),
      m_stressed_portfolio_value(0.0), m_pnl(0.0),
      m_positions(std::move(positions))
{
    for (const auto& position : m_positions)
    {
        m_stressed_portfolio_value += position.stressedValue();
        m_pnl += position.pnl();
    }

    if (!std::isfinite(m_stressed_portfolio_value) || !std::isfinite(m_pnl))
    {
        throw std::overflow_error("scenario result is not finite");
    }
}

std::uint64_t ScenarioResult::scenarioId() const noexcept
{
    return m_scenario_id;
}

const std::string& ScenarioResult::scenarioName() const noexcept
{
    return m_scenario_name;
}

double ScenarioResult::currentPortfolioValue() const noexcept
{
    return m_current_portfolio_value;
}

double ScenarioResult::stressedPortfolioValue() const noexcept
{
    return m_stressed_portfolio_value;
}

double ScenarioResult::pnl() const noexcept
{
    return m_pnl;
}

double ScenarioResult::loss() const noexcept
{
    return -m_pnl;
}

double ScenarioResult::absoluteLoss() const noexcept
{
    return std::max(0.0, -m_pnl);
}

std::span<const PositionRepricing> ScenarioResult::positions() const noexcept
{
    return m_positions;
}

ScenarioResult
ScenarioRepricer::reprice(const PortfolioValuation& current_portfolio,
                          const Scenario& scenario,
                          const std::uint64_t scenario_id) const
{
    std::vector<PositionRepricing> positions;
    positions.reserve(current_portfolio.positions().size());

    for (const auto& position : current_portfolio.positions())
    {
        const Position& portfolio_position = position.position();
        const auto relative_shock =
            scenario.priceShockFor(portfolio_position.symbol());
        
        if (!relative_shock.has_value())
        {
            throw std::invalid_argument(
                "scenario '" + std::string{scenario.name()} +
                "' is missing a price shock for position symbol: " +
                portfolio_position.symbol());
        }

        const auto stressed_price = scenario.stressedPriceFor(
            portfolio_position.symbol(), position.currentPrice());
        
        if (!stressed_price.has_value())
        {
            throw std::logic_error(
                "validated scenario unexpectedly has no stressed price");
        }

        const double stressed_value =
            portfolio_position.quantity() * *stressed_price;
        const double pnl = stressed_value - position.marketValue();
        
        if (!std::isfinite(stressed_value) || !std::isfinite(pnl))
        {
            throw std::overflow_error(
                "stressed value is not finite for position symbol: " +
                portfolio_position.symbol());
        }

        positions.emplace_back(
            portfolio_position.symbol(), portfolio_position.quantity(),
            position.currentPrice(), *relative_shock, *stressed_price,
            position.marketValue(), stressed_value, pnl);
    }

    return ScenarioResult{scenario_id, std::string{scenario.name()},
                          current_portfolio.totalValue(), std::move(positions)};
}

} // namespace htpste
