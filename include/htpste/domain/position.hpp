#ifndef POSITION_H
#define POSITION_H

#include <string>

namespace htpste
{
// An equity holding. Market data lives outside this type.
class Position final
{
  public:
    Position(std::string symbol, double quantity);

    [[nodiscard]] const std::string& symbol() const noexcept;
    [[nodiscard]] double quantity() const noexcept;
    [[nodiscard]] bool isLong() const noexcept;
    [[nodiscard]] bool isShort() const noexcept;

  private:
    std::string m_symbol;
    double m_quantity;
};

} // namespace htpste

#endif // POSITION_H
