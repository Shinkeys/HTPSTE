#include "htpste/domain/position.hpp"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <stdexcept>
#include <utility>

namespace htpste {
namespace {

[[nodiscard]] bool isBlank(const std::string& value) {
    return value.empty() || std::ranges::all_of(value, [](const unsigned char character) {
        return std::isspace(character) != 0;
    });
}

}  // namespace

Position::Position(std::string symbol, const double quantity)
    : m_symbol(std::move(symbol)), m_quantity(quantity) {
    if (isBlank(m_symbol)) {
        throw std::invalid_argument("position symbol must not be blank");
    }
    if (!std::isfinite(m_quantity)) {
        throw std::invalid_argument("position quantity must be finite");
    }
    if (m_quantity == 0.0) {
        throw std::invalid_argument("position quantity must not be zero");
    }
}

const std::string& Position::symbol() const noexcept {
    return m_symbol;
}

double Position::quantity() const noexcept {
    return m_quantity;
}

bool Position::isLong() const noexcept {
    return m_quantity > 0.0;
}

bool Position::isShort() const noexcept {
    return m_quantity < 0.0;
}

}  // namespace htpste
