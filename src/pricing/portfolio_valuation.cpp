#include "htpste/pricing/portfolio_valuation.hpp"

#include <cmath>
#include <stdexcept>
#include <utility>

namespace htpste
{
PositionValuation::PositionValuation(Position position,
                                     const double current_price)
    : m_position(std::move(position)), m_current_price(current_price)
{
    if (!std::isfinite(m_current_price) || m_current_price < 0.0)
    {
        throw std::invalid_argument(
            "position valuation price must be finite and non-negative");
    }
    if (!std::isfinite(marketValue()))
    {
        throw std::overflow_error(
            "current market value is not finite for position symbol: " +
            m_position.symbol());
    }
}

const Position& PositionValuation::position() const noexcept
{
    return m_position;
}

double PositionValuation::currentPrice() const noexcept
{
    return m_current_price;
}

double PositionValuation::marketValue() const noexcept
{
    return m_position.quantity() * m_current_price;
}

PortfolioValuation::PortfolioValuation(std::vector<PositionValuation> positions)
    : m_positions(std::move(positions)), m_total_value(0.0),
      m_long_exposure(0.0), m_short_exposure(0.0)
{
    for (const auto& position : m_positions)
    {
        const double market_value = position.marketValue();
        m_total_value += market_value;

        if (market_value > 0.0)
        {
            m_long_exposure += market_value;
        }
        else
        {
            m_short_exposure += -market_value;
        }
    }

    if (!std::isfinite(m_total_value) || !std::isfinite(m_long_exposure) ||
        !std::isfinite(m_short_exposure))
    {
        throw std::overflow_error("portfolio valuation is not finite");
    }
}

std::span<const PositionValuation>
PortfolioValuation::positions() const noexcept
{
    return m_positions;
}

double PortfolioValuation::totalValue() const noexcept
{
    return m_total_value;
}

double PortfolioValuation::longExposure() const noexcept
{
    return m_long_exposure;
}

double PortfolioValuation::shortExposure() const noexcept
{
    return m_short_exposure;
}

double PortfolioValuation::grossExposure() const noexcept
{
    return m_long_exposure + m_short_exposure;
}

PortfolioValuation
PortfolioValuator::value(const Portfolio& portfolio,
                         const MarketState& market_state) const
{
    std::vector<PositionValuation> valuations;
    valuations.reserve(portfolio.size());

    for (const auto& position : portfolio.positions())
    {
        const auto current_price = market_state.priceFor(position.symbol());
        if (!current_price.has_value())
        {
            throw std::invalid_argument(
                "missing current price for position symbol: " +
                position.symbol());
        }

        valuations.emplace_back(position, *current_price);
    }

    return PortfolioValuation{std::move(valuations)};
}

} // namespace htpste
