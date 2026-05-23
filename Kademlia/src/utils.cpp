//
// Created by L on 10.10.2025.
//

#include "utils.h"

std::bitset<kademlia::constants::kNodeIdSize> kademlia::utils::hexToBitset(const std::string &hex_str) {
    std::bitset<kademlia::constants::kNodeIdSize> b;
    for (char c: hex_str) {
        b <<= 4;

        if (c >= '0' && c <= '9') {
            b |= (c - '0');
        } else if (c >= 'a' && c <= 'f') {
            b |= (c - 'a' + 10);
        } else if (c >= 'A' && c <= 'F') {
            b |= (c - 'A' + 10);
        }
    }
    return b;
}

std::string kademlia::utils::bitsetToHex(const std::bitset<kademlia::constants::kNodeIdSize> &b) {
    std::string hex_str;
    hex_str.reserve(32);

    const char hex_chars[] = "0123456789ABCDEF";

    for (int i = 124; i >= 0; i -= 4) {
        uint8_t nibble = 0;

        if (b.test(i + 3)) nibble |= 8;
        if (b.test(i + 2)) nibble |= 4;
        if (b.test(i + 1)) nibble |= 2;
        if (b.test(i))     nibble |= 1;

        hex_str.push_back(hex_chars[nibble]);
    }

    return hex_str;
}
