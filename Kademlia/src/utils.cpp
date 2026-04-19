//
// Created by L on 10.10.2025.
//

#include "utils.h"

std::bitset<128> kademlia::utils::hexToBitset(const std::string &hex_str) {
    std::bitset<128> b;
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
