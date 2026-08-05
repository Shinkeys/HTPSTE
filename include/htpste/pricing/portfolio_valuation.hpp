#ifndef PORTFOLIO_VALUATION_H
#define PORTFOLIO_VALUATION_H

#include "htpste/domain/market_state.hpp"
#include "htpste/domain/portfolio.hpp"

#include <span>
#include <string>
#include <vector>

namespace htpste
{
class PositionValuation final
{
  public:
    PositionValuation(Position position, double current_price);

    [[nodiscard]] const Position& position() const noexcept;
    [[nodiscard]] double currentPrice() const noexcept;
    [[nodiscard]] double marketValue() const noexcept;

  private:
    Position m_position;
    double m_current_price;
};

class PortfolioValuation final
{
  public:
    explicit PortfolioValuation(std::vector<PositionValuation> positions);

    [[nodiscard]] std::span<const PositionValuation> positions() const noexcept;
    [[nodiscard]] double totalValue() const noexcept;
    [[nodiscard]] double longExposure() const noexcept;
    [[nodiscard]] double shortExposure() const noexcept;
    [[nodiscard]] double grossExposure() const noexcept;

  private:
    std::vector<PositionValuation> m_positions;
    double m_total_value;
    double m_long_exposure;
    double m_short_exposure;
};

class PortfolioValuator final
{
  public:
    [[nodiscard]] PortfolioValuation
    value(const Portfolio& portfolio, const MarketState& market_state) const;
};

} // namespace htpste

#endif // PORTFOLIO_VALUATION_H
