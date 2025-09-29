//
// Created by L on 25.09.2025.
//
#include "routing_table.h"

// TODO: may be get stored data from a file or smth
kademlia::RoutingTable::RoutingTable() {}

bool kademlia::RoutingTable::storeNode(const NodeEntry &input_node) {
    return false;
}

std::vector<NodeEntry> kademlia::RoutingTable::findNode(const std::array<unsigned char, 16> &node_id) {
    return std::vector<NodeEntry>();
}

