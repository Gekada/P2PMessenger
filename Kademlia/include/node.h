//
// Created by L on 03.08.2025.
//

#ifndef P2PMESSENGER_NODE_H
#define P2PMESSENGER_NODE_H

#include <QObject>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QTcpServer>

#include <bitset>

#include "message.pb.h"
#include "constants.h"
#include "routing_table.h"
#include "message_builder.h"
#include "utils.h"
#include "lookup_task_base.h"
#include "db_controller.h"

namespace kademlia {
    //TODO: either make immovable or shared_from_this
    class Node : public QObject {
    Q_OBJECT

    public:
        using CallbackTable = std::unordered_map<
                proto::MessageType, std::function<void(const proto::Message &, Node &)>>;

        explicit Node(QObject *parent = nullptr);

        explicit Node(uint16_t input_udp_port, QObject *parent = nullptr);


        void processMessage(const proto::Message &input_message);

        void findNode(std::bitset<constants::kNodeIdSize> target_id);

        void findValue(std::bitset<constants::kNodeIdSize> target_id);

        const std::bitset<constants::kNodeIdSize> &getId();

        void
        sendStore(std::bitset<constants::kNodeIdSize> key, const std::string &value,
                  std::bitset<constants::kNodeIdSize> receiver_id);

    public slots:

        void onReceive();

        void startNode(bool is_bootstrap = false, bool is_fake = false);

        void fetchContactsFromDb();

        void fetchMessagesFromDb(const QString &peerId);

        void fetchMyId();

    signals:

        // In this case it signals that the first ping with bootstrap is done
        void finishedBootstrap();

        void receivedMessage(proto::DataInfo message);

        void contactsFetched(const QVariantList &contactsList);

        void messagesFetched(const QVariantList &messagesList);

        void myIdFetched(const QString& id);

    private:
        void initSocket();

        bool isNewConnection();

        void initNodeId();

        std::string generateNodeId();

        void pushRequestToMap(const proto::Message &message);

        void bootstrap();

        void sendRequest(const proto::Message &proto_message, const QHostAddress &receiver, int port);

        void registerNestedNodes(const proto::Message &proto_message);

        MessageBuilder builder_;

        std::unique_ptr<data::DBController> db_controller_;

        std::bitset<constants::kNodeIdSize> node_id_;
        RoutingTable r_table_;
        bool is_bootstrapped_ = false;

        // TODO: don't forget to delete after testing
        bool is_fake_;

        const uint16_t kUdpPort;
        //const uint16_t kTcpPort = 5225;
        std::unique_ptr<QUdpSocket> udp_socket_;
        QTcpSocket tcp_socket_;
        QTcpServer tcp_server_;

        // Here we save each sent rpc request, which awaits a response, the string is an rpc id
        std::unordered_map<std::string, proto::MessageType> requests_map_;
        // A map which couples LookupValue and LookupNode tasks (rpc id to object)
        std::unordered_map<std::string, std::shared_ptr<ILookupTask>> lookup_task_map_;

        static const CallbackTable callbacks_;
    };
}
#endif //P2PMESSENGER_NODE_H
