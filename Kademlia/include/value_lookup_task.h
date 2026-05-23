//
// Created by L on 28.04.2026.
//

#ifndef P2PMESSENGER_VALUE_LOOKUP_TASK_H
#define P2PMESSENGER_VALUE_LOOKUP_TASK_H
#include "lookup_task_base.h"

namespace kademlia {
    class ValueLookupTask final : public ILookupTask {
    Q_OBJECT

    using DataEntry = proto::DataInfo;
    public:
        ValueLookupTask(RoutingTable &routing_table, std::bitset<128> target_id, std::bitset<128> owner_id);
        ~ValueLookupTask() final = default;

        void onResponseReceived(const proto::Message &response) final;

    signals:

        void valueSearchSucceeded(std::vector<DataEntry> found_data);
        void valueSearchFailed(std::vector<NodeEntry> found_nodes);

    private:
        virtual proto::Message buildRequest(const std::string &receiver_id);
        std::vector<DataEntry> found_data_;
    };
}
#endif //P2PMESSENGER_VALUE_LOOKUP_TASK_H
