//
// Created by L on 09.10.2025.
//

#ifndef P2PMESSENGER_UTILS_H
#define P2PMESSENGER_UTILS_H

#include <array>
#include <bitset>

#include <magic_enum/magic_enum.hpp>
#include <fmt/format.h>

#include "constants.h"

template<std::size_t N>
bool operator<(const std::bitset<N> &first, const std::bitset<N> &second) {
    for (int i = N - 1; i >= 0; i--) {
        if (first[i] ^ second[i]) return second[i];
    }
    return false;
}

template<typename Enum>
struct fmt::formatter<Enum, std::enable_if_t<std::is_enum_v<Enum>, char>>
        : fmt::formatter<std::string_view> {
    template<typename FormatContext>
    auto format(const Enum e, FormatContext &ctx) const {
        return fmt::formatter<std::string_view>::format(magic_enum::enum_name(e),
                                                        ctx);
    }
};

namespace kademlia::utils {

    std::bitset<128> hexToBitset(const std::string &hex_str);

}
#endif //P2PMESSENGER_UTILS_H

