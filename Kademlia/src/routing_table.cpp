//
// Created by L on 25.09.2025.
//
#include "routing_table.h"
#include "utils.h"

kademlia::RoutingTable::RoutingTable(std::bitset<constants::kNodeIdSize> owner_id) : owner_id_(owner_id) {}

bool
kademlia::RoutingTable::storeNode(const NodeEntry &input_node) {
    auto nodes_distance = input_node.node_id_ ^ owner_id_;
    for (int i = nodes_distance.size() - 1; i >= 0; i--) {
        if (nodes_distance[i]) {
            if (k_buckets_[i].size() < constants::kBucketLimit) {
                if (eraseDuplicate(k_buckets_[i], input_node)) {
                    return true;
                }
                k_buckets_[i].push_back(input_node);
                return true;
            }
            // TODO: write a second branch with peers activity check
        }
    }
    return false;
}

std::vector<kademlia::NodeEntry>
kademlia::RoutingTable::findKNodes(const std::bitset<constants::kNodeIdSize> &searching_id) {
    std::vector<kademlia::NodeEntry> found_nodes;
    auto nodes_distance = searching_id ^ owner_id_;
    short i = nodes_distance.size() - 1;
    while (i >= 0) {
        if (nodes_distance[i]) {
            std::copy(k_buckets_[i].begin(), k_buckets_[i].end(), std::back_inserter(found_nodes));
            break;
        }
        i--;
    }

    if (found_nodes.size() == constants::kBucketLimit) {
        return found_nodes;
    }

    short left_edge = i - 1;
    short right_edge = i + 1;
    while (found_nodes.size() < constants::kBucketLimit && (left_edge > 0 || right_edge < k_buckets_.size())) {
        if (left_edge >= 0) {
            std::copy(k_buckets_[left_edge].begin(), k_buckets_[left_edge].end(),
                      std::back_inserter(found_nodes));
            left_edge--;
        }
        if (right_edge < k_buckets_.size()) {
            std::copy(k_buckets_[right_edge].begin(), k_buckets_[right_edge].end(),
                      std::back_inserter(found_nodes));
            right_edge++;
        }
    }

    std::sort(found_nodes.begin(), found_nodes.end(), [searching_id](const auto &first, const auto &second) {
        return (first.getId() ^ searching_id) < (second.getId() ^ searching_id);
    });

    if (found_nodes.size() > constants::kBucketLimit) {
        found_nodes.resize(constants::kBucketLimit);
    }

    return found_nodes;
}

bool inline kademlia::RoutingTable::eraseDuplicate(std::vector<NodeEntry> &k_bucket, const NodeEntry &input_node) {
    auto pos = std::find_if(k_bucket.cbegin(), k_bucket.cend(), [input_node](const auto &el) {
        return el.node_id_ == input_node.node_id_;
    });
    if (pos != k_bucket.end()) {
        if (pos == k_bucket.end() - 1) {
            return true;
        }
        k_bucket.erase(pos);
    }
    return false;
}

std::optional<kademlia::NodeEntry>
kademlia::RoutingTable::findNode(const std::bitset<constants::kNodeIdSize> &searching_id) {
    auto nodes_distance = searching_id ^ owner_id_;
    for (int i = nodes_distance.size() - 1; i >= 0; i--) {
        if (nodes_distance[i]) {
            auto it = std::find_if(k_buckets_[i].begin(), k_buckets_[i].end(), [searching_id](const auto &elem) {
                return elem.node_id_ == searching_id;
            });
            if (it != k_buckets_[i].end()) {
                return {*it};
            }
            break;
        }
    }
    return std::nullopt;
}
