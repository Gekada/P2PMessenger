#ifndef P2PMESSENGER_NODE_ENTRY_H
#define P2PMESSENGER_NODE_ENTRY_H

#include <QHostAddress>

#include <array>
#include <bitset>

#include "constants.h"

namespace kademlia {
    struct NodeEntry {
        uint16_t port_;
        QHostAddress ip_address_;
        std::bitset<constants::kNodeIdSize> node_id_;
    };
}
#endif //P2PMESSENGER_NODE_ENTRY_H
