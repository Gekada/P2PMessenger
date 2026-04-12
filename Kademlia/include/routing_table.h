//
// Created by L on 25.09.2025.
//

#ifndef P2PMESSENGER_ROUTING_TABLE_H
#define P2PMESSENGER_ROUTING_TABLE_H

#include <array>
#include <vector>
#include <optional>

#include "node_entry.h"

namespace kademlia {
    class RoutingTable {
    public:
        RoutingTable(std::bitset<constants::kNodeIdSize> owner_id);
        bool storeNode(const NodeEntry &input_node, const std::bitset<constants::kNodeIdSize> &owner_id);
        std::vector<NodeEntry> findKNodes(const std::bitset<constants::kNodeIdSize> &searching_id);
        std::optional<NodeEntry> findNode(const std::bitset<constants::kNodeIdSize> &searching_id);


    private:
        //erases a duplicate if we are trying to put an already existent node which in not already at the end
        bool inline eraseDuplicate(std::vector<NodeEntry> &k_bucket, const NodeEntry &input_node);
        //stores nodes from furthest to nearest (k_buckets[kNodeIdSize] - the nearest node);
        std::array<std::vector<NodeEntry>, constants::kNodeIdSize> k_buckets_;
        const std::bitset<constants::kNodeIdSize> owner_id_;
    };
}
#endif //P2PMESSENGER_ROUTING_TABLE_H
