//
// Created by L on 03.08.2025.
//
#include <QNetworkDatagram>
#include "node.h"

const kademlia::Node::CallbackTable kademlia::Node::callbacks_{
        {proto::MessageType::PING, [](const proto::Message &message, Node &node) {
            std::cout << "Node Ping rpc :\')";
        }}
};

//TODO: move magic number for port into config file/constexpr field as a default port number
kademlia::Node::Node(QObject *parent) : Node(1337) {}

kademlia::Node::Node(uint16_t input_udp_port, QObject *parent) : QObject(parent), kUdpPort(input_udp_port) {
    initSocket();
}

void kademlia::Node::initSocket() {
    qDebug("Socket init");
    udp_socket_ = std::make_unique<QUdpSocket>();
    auto result = udp_socket_->bind(QHostAddress::LocalHost, kUdpPort);

    //TODO: change port by some criteria in case the default is already in use
    if (!result){
        qDebug("Socket didn't bind yopta");
    }

    QObject::connect(udp_socket_.get(), &QUdpSocket::readyRead,
                     this, &kademlia::Node::onReceive);
}

void kademlia::Node::onReceive() {
    qDebug("OnRecieve");
    while (udp_socket_->hasPendingDatagrams()) {
        QNetworkDatagram datagram = udp_socket_->receiveDatagram();
        qDebug("Received a message");
    }
}

void kademlia::Node::processMessage(const proto::Message &input_message) {
    if (udp_socket_->hasPendingDatagrams()) {
        QNetworkDatagram datagram = udp_socket_->receiveDatagram();
        qDebug() << datagram.data();
    } else {
        qDebug("damn");
    }
}


