//
// Created by L on 03.08.2025.
//

#ifndef P2PMESSENGER_NODE_H
#define P2PMESSENGER_NODE_H

#include <QObject>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QTcpServer>

#include "message.pb.h"

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


    public slots:

        void onReceive();

    private:
        void initSocket();


        std::array<unsigned char, 16> node_id_;
        // Put it into conf file later
        const uint16_t kUdpPort ;
        //const uint16_t kTcpPort = 5225;
        std::unique_ptr<QUdpSocket> udp_socket_;
        QTcpSocket tcp_socket_;
        QTcpServer tcp_server_;

        static const CallbackTable callbacks_;
    };
}
#endif //P2PMESSENGER_NODE_H
