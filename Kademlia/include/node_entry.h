#ifndef P2PMESSENGER_NODE_ENTRY_H
#define P2PMESSENGER_NODE_ENTRY_H

#include <QHostAddress>

#include <array>
#include <bitset>

#include "constants.h"

namespace kademlia {
    struct NodeEntry {
        int port_;
        QHostAddress ip_address_;
        std::bitset<constants::kNodeIdSize> node_id_;

        [[nodiscard]] std::bitset<constants::kNodeIdSize> getId() const{
            return node_id_;
        }
    };
}
#endif //P2PMESSENGER_NODE_ENTRY_H
