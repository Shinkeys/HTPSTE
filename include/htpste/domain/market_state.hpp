#ifndef MARKET_STATE_H
#define MARKET_STATE_H

#include "htpste/common/transparent_string_hash.hpp"

#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

namespace htpste
{
using PriceMap = std::unordered_map<std::string, double, TransparentStringHash,
                                    std::equal_to<>>;
using ParameterMap = std::unordered_map<std::string, double,
                                        TransparentStringHash, std::equal_to<>>;

struct ModelParameters final
{
    ParameterMap values;
};

class MarketState final
{
  public:
    explicit MarketState(PriceMap prices, ModelParameters modelParameters = {});

    [[nodiscard]] std::optional<double>
    priceFor(std::string_view symbol) const noexcept;
    [[nodiscard]] std::optional<double>
    parameter(std::string_view name) const noexcept;

    [[nodiscard]] const PriceMap& prices() const noexcept;
    [[nodiscard]] const ModelParameters& modelParameters() const noexcept;

  private:
    PriceMap m_prices;
    ModelParameters m_model_parameters;
};

} // namespace htpste

#endif // MARKET_STATE_H
