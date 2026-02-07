//
// Created by L on 25.09.2025.
//
#include "routing_table.h"

// TODO: maybe get stored data from a file or smth
kademlia::RoutingTable::RoutingTable() {}

// TODO: account for situations when the node is already stored (move it to tail of its bucket)
bool
kademlia::RoutingTable::storeNode(const NodeEntry &input_node, const std::bitset<constants::kNodeIdSize> &owner_id) {
    auto nodes_distance = input_node.node_id_ ^ owner_id;
    for (int i = 0; i < nodes_distance.size(); i++) {
        if (nodes_distance[i]) {
            if (k_buckets_[i].size() < constants::kBucketLimit) {
                // TODO: there should be some sort by recency or smth
                k_buckets_[i].emplace_back(input_node);
                return true;
            }
            // TODO: write a second branch with peers activity check
        }
    }
    return false;
}

//I am terrified of how many todos I've made when the project is like 150 lines of code...
std::vector<kademlia::NodeEntry> kademlia::RoutingTable::findNode(const std::bitset<constants::kNodeIdSize> &node_id,
                                                                  const std::bitset<constants::kNodeIdSize> &owner_id) {
    std::vector<kademlia::NodeEntry> found_nodes;
    auto nodes_distance = node_id ^ owner_id;
    short i = 0;
    while (i < nodes_distance.size()) {
        if (nodes_distance[i]) {
            std::copy(k_buckets_[i].begin(), k_buckets_[i].begin(), found_nodes.begin());
            break;
        }
        i++;
    }
    if (found_nodes.size() < constants::kBucketLimit) {
        short left_edge = i - 1;
        short right_edge = i + 1;
        while ((left_edge >= 0 || right_edge < k_buckets_.size()) && found_nodes.size() < constants::kBucketLimit) {
            unsigned short left_it = 0;
            unsigned short right_it = 0;
            while (left_it < constants::kBucketLimit || right_it < constants::kBucketLimit) {

            }
            left_edge++;
            right_edge++;
        }
    }
    return found_nodes;
}
