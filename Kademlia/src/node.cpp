//
// Created by L on 03.08.2025.
//
#include <QNetworkDatagram>

#include <nlohmann/json.hpp>

#include <fstream>

#include "node.h"

const kademlia::Node::CallbackTable kademlia::Node::callbacks_{
        {proto::MessageType::PING, [](const proto::Message &message, Node &node) {
            std::cout << "Node Ping rpc :\')";
        }}
};

kademlia::Node::Node(QObject *parent) : Node(kademlia::constants::kDefaultUdpPort) {}

kademlia::Node::Node(uint16_t input_udp_port, QObject *parent) : QObject(parent), kUdpPort(input_udp_port),
                                                                 r_table_(node_id_) {
    initSocket();
}

void kademlia::Node::initSocket() {
    udp_socket_ = std::make_unique<QUdpSocket>();
    auto result = udp_socket_->bind(QHostAddress::LocalHost, kUdpPort);

    //TODO: change port by some criteria in case the default is already in use
    if (!result) {
        qDebug("Socket didn't bind yopta");
    }

    QObject::connect(udp_socket_.get(), &QUdpSocket::readyRead,
                     this, &kademlia::Node::onReceive);
}

// This code would be refactored in some time
void kademlia::Node::onReceive() {
    qDebug("OnRecieve");
    QByteArray message_buffer;
    while (udp_socket_->hasPendingDatagrams()) {
        QNetworkDatagram datagram = udp_socket_->receiveDatagram();
        message_buffer.push_back(datagram.data());
        qDebug("Received a message");
    }
    proto::Message received_message;
    if (received_message.ParseFromArray(message_buffer.data(), message_buffer.size())) {
        processMessage(received_message);
    }
}

void kademlia::Node::processMessage(const proto::Message &input_message) try {
    auto callback = callbacks_.find(input_message.type());
    if (callback == callbacks_.end()) {
        qWarning() << "Invalid message type: " << input_message.type();
        return;
    }
    callback->second(input_message, *this);
} catch (const std::exception &ex) {
    qWarning() << "Caught an exception: " << ex.what();
}

void kademlia::Node::bootstrap() {

}

bool kademlia::Node::isNewConnection() {
    std::ifstream config_input(constants::confPath);
    nlohmann::json data = nlohmann::json::parse(config_input);

    if (data.find("nodeId") == data.end()){
        return false;
    }
    return true;
}

void kademlia::Node::generateNodeId() {
    std::ifstream config_input(constants::confPath);
    std::ofstream config_output(constants::confPath);
    nlohmann::json data = nlohmann::json::parse(config_input);
}


