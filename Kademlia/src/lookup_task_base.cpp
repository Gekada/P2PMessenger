//
// Created by L on 23.04.2026.
//

#include "lookup_task_base.h"
#include "routing_table.h"
#include "constants.h"

kademlia::ILookupTask::ILookupTask(kademlia::RoutingTable &routing_table, std::bitset<128> target_id,
                                   std::bitset<128> owner_id) : routing_table_(routing_table),
                                                                target_id_(target_id),
                                                                owner_id_(owner_id.to_string()) {}

void kademlia::ILookupTask::start() {
    saved_nodes_ = routing_table_.findKNodes(target_id_);

    if (saved_nodes_.empty()) {
        qDebug("The routing table is empty, stopping the lookup");
        emit taskFinished();
        return;
    }

    sendNextRequests();
}

void kademlia::ILookupTask::sendNextRequests() {
    auto request_to_send = constants::kParallelismLimit - active_requests_;
    for (const auto &node: saved_nodes_) {
        if (request_to_send == 0) {
            break;
        }
        if (!queried_nodes_.contains(node.node_id_.to_string()) &&
            node.node_id_ != std::bitset<constants::kNodeIdSize>(owner_id_)) {
            auto request = this->buildRequest(node.node_id_.to_string());
            emit requestCreated(request, node.ip_address_, node.port_);
            active_requests_++;
            request_to_send--;
            queried_nodes_.insert(node.node_id_.to_string());
        }
    }
    // If all nodes have already been queried
    if (active_requests_ == 0 && !is_complete_) {
        emit taskFinished();
    }
}

void kademlia::ILookupTask::requestTimedOut() {
    active_requests_--;
    if (checkFirstK()) {
        sendNextRequests();
    } else if (active_requests_ == 0 && !is_complete_) {
        is_complete_ = true;
        emit taskFinished();
    }

}

bool kademlia::ILookupTask::checkFirstK() {
    for (int i = 0; i < saved_nodes_.size() && i < constants::kBucketLimit; i++) {
        if (!queried_nodes_.contains(saved_nodes_[i].node_id_.to_string())) {
            return true;
        }
    }
    return false;
}

