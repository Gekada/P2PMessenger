//
// Created by L on 21.04.2026.
//

#ifndef P2PMESSENGER_NODE_LOOKUP_TASK_H
#define P2PMESSENGER_NODE_LOOKUP_TASK_H

#include "lookup_task_base.h"

namespace kademlia {
    class NodeLookupTask final : public ILookupTask {
    Q_OBJECT

    public:
        NodeLookupTask(RoutingTable &routing_table, std::bitset<128> target_id, std::bitset<128> owner_id);
        ~NodeLookupTask() final = default;

        void onResponseReceived(const proto::Message &response) final;

    signals:

        void nodeSearchFinished(std::vector<NodeEntry> found_nodes);

    private:
        virtual proto::Message buildRequest(const std::string &receiver_id);
    };
}
#endif //P2PMESSENGER_NODE_LOOKUP_TASK_H
