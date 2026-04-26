//
// Created by L on 23.04.2026.
//

#ifndef P2PMESSENGER_LOOKUP_TASK_H
#define P2PMESSENGER_LOOKUP_TASK_H

#include <QObject>
#include <bitset>
#include <vector>
#include <unordered_set>
#include <memory>

#include "node_entry.h"
#include "message.pb.h"
#include "message_builder.h"

namespace kademlia {
    class RoutingTable;

    class ILookupTask : public QObject {
    Q_OBJECT

    public:
        ILookupTask(RoutingTable &routing_table, std::bitset<128> target_id, std::bitset<128> owner_id);

        virtual ~ILookupTask() = default;

        void start();

        virtual void onResponseReceived(const proto::Message &response) = 0;

        void requestTimedOut();


    signals:

        void taskFinished();

        void requestCreated(const proto::Message &request, const QHostAddress &receiver, int port);

    protected:

        // in here we state the particular find rpc (node or value)
        virtual proto::Message buildRequest(const std::string &receiver_id) = 0;

        void sendNextRequests();

        //checks if the there are unqueried nodes in the first k records (to stop the search)
        bool checkFirstK();

        MessageBuilder builder_;

        RoutingTable &routing_table_;
        std::bitset<128> target_id_;
        // we wont do any xor, so it's better to store as string
        std::string owner_id_;

        std::vector<NodeEntry> saved_nodes_;
        std::unordered_set<std::string> queried_nodes_;
        int active_requests_ = 0;
        bool is_complete_ = false;
    };

}
#endif //P2PMESSENGER_LOOKUP_TASK_H
