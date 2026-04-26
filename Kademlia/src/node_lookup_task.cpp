//
// Created by L on 21.04.2026.
//
#include "node_lookup_task.h"


kademlia::NodeLookupTask::NodeLookupTask(kademlia::RoutingTable &routing_table, std::bitset<128> target_id,
                                         std::bitset<128> owner_id) : ILookupTask(routing_table, target_id, owner_id) {
    QObject::connect(this, &NodeLookupTask::taskFinished, [this]() {
        if (saved_nodes_.size() > constants::kBucketLimit) {
            saved_nodes_.resize(constants::kBucketLimit);
        }
        emit nodeSearchFinished(saved_nodes_);
    });
}

void kademlia::NodeLookupTask::onResponseReceived(const proto::Message &response) {
    active_requests_--;
    auto found_nodes = response.closest_nodes();
    for (auto node: found_nodes) {
        if (std::find_if(saved_nodes_.begin(), saved_nodes_.end(), [node](const auto &elem) {
            return elem.node_id_.to_string() == node.node_id();
        }) == saved_nodes_.end()) {
            saved_nodes_.push_back(
                    {node.port(), QHostAddress(node.ip().data()), std::bitset<constants::kNodeIdSize>(node.node_id())});
        }
    }
    std::sort(saved_nodes_.begin(), saved_nodes_.end(), [this](const auto &first, const auto &second) {
        return (first.getId() ^ target_id_) < (second.getId() ^ target_id_);
    });
    if (checkFirstK()) {
        sendNextRequests();
    }
    else if (active_requests_ == 0 && !is_complete_) {
        is_complete_ = true;
        emit taskFinished();
    }
}

proto::Message kademlia::NodeLookupTask::buildRequest(const std::string &receiver_id) {
    return builder_.setType(proto::MessageType::FIND_NODE).setSender(owner_id_).setReciever(receiver_id).setPayload(
            target_id_.to_string()).buildUnwrapped();
}


