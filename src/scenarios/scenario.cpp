#include "htpste/scenarios/scenario.hpp"

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

void validatePriceShocks(const PriceShockMap& shocks) {
    for (const auto& [symbol, shock] : shocks) {
        if (isBlank(symbol)) {
            throw std::invalid_argument("scenario symbol must not be blank");
        }
        if (!std::isfinite(shock) || shock < -1.0) {
            throw std::invalid_argument(
                "relative price shocks must be finite and no smaller than -1"
            );
        }
    }
}

void validateParameterShifts(const ParameterShiftMap& shifts) {
    for (const auto& [name, shift] : shifts) {
        if (isBlank(name)) {
            throw std::invalid_argument("scenario parameter name must not be blank");
        }
        if (!std::isfinite(shift)) {
            throw std::invalid_argument("scenario parameter shifts must be finite");
        }
    }
}

}  // namespace

Scenario::Scenario(PriceShockMap price_shocks, ParameterShiftMap parameter_shifts)
    : m_price_shocks(std::move(price_shocks)),
      m_parameter_shifts(std::move(parameter_shifts)) {
    validatePriceShocks(m_price_shocks);
    validateParameterShifts(m_parameter_shifts);
}

std::optional<double> Scenario::priceShockFor(const std::string_view symbol) const noexcept {
    const auto shock = m_price_shocks.find(symbol);
    if (shock == m_price_shocks.end()) {
        return std::nullopt;
    }
    return shock->second;
}

std::optional<double> Scenario::parameterShiftFor(const std::string_view name) const noexcept {
    const auto shift = m_parameter_shifts.find(name);
    if (shift == m_parameter_shifts.end()) {
        return std::nullopt;
    }
    return shift->second;
}

std::optional<double> Scenario::stressedPriceFor(
    const std::string_view symbol,
    const double current_price
) const {
    if (!std::isfinite(current_price) || current_price < 0.0) {
        throw std::invalid_argument("current price must be finite and non-negative");
    }

    const auto shock = priceShockFor(symbol);
    if (!shock.has_value()) {
        return std::nullopt;
    }

    const double stressed_price = current_price * (1.0 + *shock);
    if (!std::isfinite(stressed_price)) {
        throw std::overflow_error("stressed price is not finite");
    }
    return stressed_price;
}

const PriceShockMap& Scenario::priceShocks() const noexcept {
    return m_price_shocks;
}

const ParameterShiftMap& Scenario::parameterShifts() const noexcept {
    return m_parameter_shifts;
}

}  // namespace htpste

