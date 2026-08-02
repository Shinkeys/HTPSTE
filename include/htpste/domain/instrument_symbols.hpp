#ifndef INSTRUMENT_SYMBOLS_H
#define INSTRUMENT_SYMBOLS_H

#include <array>
#include <string_view>

namespace htpste::instrument_symbols {

inline constexpr std::string_view gold{"XAU"};
inline constexpr std::string_view nvidia{"NVDA"};
inline constexpr std::string_view apple{"AAPL"};
inline constexpr std::string_view alphabet{"GOOGL"};
inline constexpr std::string_view microsoft{"MSFT"};
inline constexpr std::string_view silver{"XAG"};
inline constexpr std::string_view amazon{"AMZN"};
inline constexpr std::string_view tsmc{"TSM"};
inline constexpr std::string_view broadcom{"AVGO"};
inline constexpr std::string_view saudi_aramco{"2222.SR"};
inline constexpr std::string_view spacex{"SPACEX"};
inline constexpr std::string_view meta{"META"};
inline constexpr std::string_view bitcoin{"BTC-USD"};
inline constexpr std::string_view tesla{"TSLA"};
inline constexpr std::string_view samsung{"005930.KS"};
inline constexpr std::string_view berkshire_hathaway{"BRK.B"};
inline constexpr std::string_view eli_lilly{"LLY"};
inline constexpr std::string_view vanguard_sp_500_etf{"VOO"};
inline constexpr std::string_view jpmorgan_chase{"JPM"};
inline constexpr std::string_view micron_technology{"MU"};
inline constexpr std::string_view sp_500{"SPX"};

inline constexpr std::array all{
    gold,
    nvidia,
    apple,
    alphabet,
    microsoft,
    silver,
    amazon,
    tsmc,
    broadcom,
    saudi_aramco,
    spacex,
    meta,
    bitcoin,
    tesla,
    samsung,
    berkshire_hathaway,
    eli_lilly,
    vanguard_sp_500_etf,
    jpmorgan_chase,
    micron_technology,
    sp_500,
};

}  // namespace htpste::instrument_symbols

#endif // INSTRUMENT_SYMBOLS_H
