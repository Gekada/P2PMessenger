//
// Created by L on 25.09.2025.
//

#ifndef P2PMESSENGER_ROUTING_TABLE_H
#define P2PMESSENGER_ROUTING_TABLE_H

#include <array>
#include <vector>

#include "node_entry.h"

namespace kademlia {
    class RoutingTable {
    public:
        RoutingTable();

        bool storeNode(const NodeEntry &input_node, const std::bitset<constants::kNodeIdSize> &owner_id);

        std::vector<NodeEntry> findNode(const std::bitset<constants::kNodeIdSize> &node_id,
                                        const std::bitset<constants::kNodeIdSize> &owner_id);

    private:
        //stores nodes from furthest to nearest (k_buckets[kNodeIdSize] - the nearest node);
        std::array<std::vector<NodeEntry>, constants::kNodeIdSize> k_buckets_;
    };
}
#endif //P2PMESSENGER_ROUTING_TABLE_H
