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
        bool storeNode(const NodeEntry& input_node);
        std::vector<NodeEntry> findNode(const std::array<unsigned char, 16>& node_id);

    private:
        std::array<std::vector<NodeEntry>, 128> k_buckets_;
        // TODO: move into config file idk
        static constexpr short kBucketLimit = 5;
    };
}
#endif //P2PMESSENGER_ROUTING_TABLE_H
