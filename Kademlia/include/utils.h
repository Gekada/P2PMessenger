//
// Created by L on 09.10.2025.
//

#ifndef P2PMESSENGER_UTILS_H
#define P2PMESSENGER_UTILS_H

#include <array>
#include <bitset>

#include "constants.h"

namespace kademlia::utils {
    unsigned long calculateDistance(const std::bitset<constants::kNodeIdSize> &from,
                                    const std::bitset<constants::kNodeIdSize> &to);
}
#endif //P2PMESSENGER_UTILS_H

