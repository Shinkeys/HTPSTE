#include "htpste/domain/portfolio.hpp"

#include <utility>

namespace htpste
{
Portfolio::Portfolio(std::vector<Position> positions)
    : m_positions(std::move(positions))
{
}

void Portfolio::addPosition(Position position)
{
    m_positions.push_back(std::move(position));
}

std::span<const Position> Portfolio::positions() const noexcept
{
    return m_positions;
}

std::size_t Portfolio::size() const noexcept
{
    return m_positions.size();
}

bool Portfolio::empty() const noexcept
{
    return m_positions.empty();
}

} // namespace htpste
