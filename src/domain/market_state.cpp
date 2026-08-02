#include "htpste/domain/market_state.hpp"

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

void validatePrices(const PriceMap& prices) {
    for (const auto& [symbol, price] : prices) {
        if (isBlank(symbol)) {
            throw std::invalid_argument("market price symbol must not be blank");
        }
        if (!std::isfinite(price) || price < 0.0) {
            throw std::invalid_argument("market prices must be finite and non-negative");
        }
    }
}

void validateParameters(const ModelParameters& modelParameters) {
    for (const auto& [name, value] : modelParameters.values) {
        if (isBlank(name)) {
            throw std::invalid_argument("model parameter name must not be blank");
        }
        if (!std::isfinite(value)) {
            throw std::invalid_argument("model parameters must be finite");
        }
    }
}

}  // namespace

MarketState::MarketState(PriceMap prices, ModelParameters modelParameters)
    : m_prices(std::move(prices)), m_model_parameters(std::move(modelParameters)) {
    validatePrices(m_prices);
    validateParameters(m_model_parameters);
}

std::optional<double> MarketState::priceFor(const std::string_view symbol) const noexcept {
    const auto price = m_prices.find(symbol);
    if (price == m_prices.end()) {
        return std::nullopt;
    }
    return price->second;
}

std::optional<double> MarketState::parameter(const std::string_view name) const noexcept {
    const auto parameter = m_model_parameters.values.find(name);
    if (parameter == m_model_parameters.values.end()) {
        return std::nullopt;
    }
    return parameter->second;
}

const PriceMap& MarketState::prices() const noexcept {
    return m_prices;
}

const ModelParameters& MarketState::modelParameters() const noexcept {
    return m_model_parameters;
}

}  // namespace htpste
