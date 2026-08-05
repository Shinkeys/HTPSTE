#ifndef PORTFOLIO_H
#define PORTFOLIO_H

#include "htpste/domain/position.hpp"

#include <cstddef>
#include <span>
#include <vector>

namespace htpste
{
class Portfolio final
{
  public:
    Portfolio() = default;
    explicit Portfolio(std::vector<Position> positions);

    void addPosition(Position position);

    [[nodiscard]] std::span<const Position> positions() const noexcept;
    [[nodiscard]] std::size_t size() const noexcept;
    [[nodiscard]] bool empty() const noexcept;

  private:
    std::vector<Position> m_positions;
};

} // namespace htpste

#endif // PORTFOLIO_H
