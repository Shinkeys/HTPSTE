#ifndef SCENARIO_H
#define SCENARIO_H

#include "htpste/common/transparent_string_hash.hpp"

#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

namespace htpste
{
using PriceShockMap =
    std::unordered_map<std::string, double, TransparentStringHash,
                       std::equal_to<>>;
using ParameterShiftMap =
    std::unordered_map<std::string, double, TransparentStringHash,
                       std::equal_to<>>;

class Scenario
{
  public:
    virtual ~Scenario() = default;

    Scenario(const Scenario&) = default;
    Scenario(Scenario&&) noexcept = default;
    Scenario& operator=(const Scenario&) = default;
    Scenario& operator=(Scenario&&) noexcept = default;

    [[nodiscard]] virtual std::string_view name() const noexcept = 0;

    [[nodiscard]] std::optional<double>
    priceShockFor(std::string_view symbol) const noexcept;
    [[nodiscard]] std::optional<double>
    parameterShiftFor(std::string_view name) const noexcept;
    [[nodiscard]] std::optional<double>
    stressedPriceFor(std::string_view symbol, double current_price) const;

    [[nodiscard]] const PriceShockMap& priceShocks() const noexcept;
    [[nodiscard]] const ParameterShiftMap& parameterShifts() const noexcept;

  protected:
    explicit Scenario(PriceShockMap price_shocks,
                      ParameterShiftMap parameter_shifts = {});

  private:
    PriceShockMap m_price_shocks;
    ParameterShiftMap m_parameter_shifts;
};

} // namespace htpste

#endif // SCENARIO_H
