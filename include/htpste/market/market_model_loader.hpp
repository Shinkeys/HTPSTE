#ifndef HTPSTE_MARKET_MODEL_LOADER_HPP
#define HTPSTE_MARKET_MODEL_LOADER_HPP

#include "htpste/market/market_model.hpp"

#include <filesystem>
#include <istream>

namespace htpste
{
class MarketModelLoader final
{
  public:
    [[nodiscard]] MarketModel
    loadFromFile(const std::filesystem::path& input_path) const;
    [[nodiscard]] MarketModel load(std::istream& input) const;
};

} // namespace htpste

#endif // HTPSTE_MARKET_MODEL_LOADER_HPP
