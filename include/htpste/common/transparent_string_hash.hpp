#ifndef TRANSPARENT_STRING_HASH_H
#define TRANSPARENT_STRING_HASH_H

#include <cstddef>
#include <functional>
#include <string_view>

namespace htpste {

struct TransparentStringHash final {
    using is_transparent = void;

    [[nodiscard]] std::size_t operator()(const std::string_view value) const noexcept {
        return std::hash<std::string_view>{}(value);
    }
};

}  // namespace htpste

#endif // TRANSPARENT_STRING_HASH_H

