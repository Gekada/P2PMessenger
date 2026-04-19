//
// Created by L on 03.08.2025.
//
#include <QNetworkDatagram>

#include <nlohmann/json.hpp>

#include <cryptopp/shake.h>
#include <cryptopp/hex.h>
#include <cryptopp/filters.h>
#include <cryptopp/cryptlib.h>

#include <fstream>

#include "node.h"

const kademlia::Node::CallbackTable kademlia::Node::callbacks_{
        {proto::MessageType::PING, [](const proto::Message &message, Node &node) {
            std::cout << "Node Ping rpc :\')\n";
        }}
};

kademlia::Node::Node(QObject *parent) : Node(kademlia::constants::kDefaultUdpPort) {}

kademlia::Node::Node(uint16_t input_udp_port, bool is_bootstrap, QObject *parent) : QObject(parent),
                                                                                    kUdpPort(input_udp_port),
                                                                                    r_table_(node_id_) {
    initSocket();
    if (is_bootstrap) {
        node_id_ = utils::hexToBitset(generateNodeId());
    } else {
        bootstrap();
    }
}

void kademlia::Node::initSocket() {
    udp_socket_ = std::make_unique<QUdpSocket>();
    auto result = udp_socket_->bind(QHostAddress::Any, kUdpPort);

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
    while (udp_socket_->hasPendingDatagrams()) {
        QNetworkDatagram datagram = udp_socket_->receiveDatagram();
        QByteArray data = datagram.data();

        proto::Message received_message;
        if (received_message.ParseFromArray(data.data(), data.size())) {
            processMessage(received_message);
        } else {
            qWarning("Failed to parse datagram into protobuf message");
        }
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
    if (isNewConnection()) {
        initNodeId();
    }

    std::ifstream config_input(constants::confPath);

    if (!config_input.is_open()) {
        qWarning() << "Couldn't open storage file";
        return;
    }
    nlohmann::json data = nlohmann::json::parse(config_input);

    std::string message = builder_.setType(proto::MessageType::PING).buildUnwrapped().SerializeAsString();
    std::string bootstrap_node_ip = data["bootstrap_node_ip"];
    int bootstrap_node_port = std::stoi(std::string(data["bootstrap_node_port"]));


    udp_socket_->writeDatagram(message.data(), message.size(), QHostAddress(bootstrap_node_ip.data()),
                               bootstrap_node_port);
}

bool kademlia::Node::isNewConnection() {
    std::ifstream config_input(constants::confPath);

    if (!config_input.is_open()) {
        qWarning() << "Couldn't open storage file";
        return false;
    }

    nlohmann::json data = nlohmann::json::parse(config_input);

    if (data.find("nodeId") == data.end()) {
        return true;
    }
    return false;
}

void kademlia::Node::initNodeId() {
    std::ifstream config_input(constants::confPath);
    nlohmann::json data = nlohmann::json::parse(config_input);
    if (!config_input.is_open()) {
        qWarning() << "Couldn't open storage file";
        return;
    }
    try {
        std::string digest = generateNodeId();
        data["node_id"] = digest;
        std::ofstream config_output(constants::confPath);
        config_output << data;
        node_id_ = utils::hexToBitset(digest);
    } catch (const std::exception &e) {
        qWarning() << "Exception during node_id creation: " << e.what();
        return;
    }
}

std::string kademlia::Node::generateNodeId() {
    std::string local_endpoint =
            udp_socket_->localAddress().toString().toStdString() + std::to_string(udp_socket_->localPort());
    std::string digest;

    short hash_size = 16;
    CryptoPP::SHAKE128 hash(hash_size);

    try {
        CryptoPP::StringSource ss(local_endpoint, true, new CryptoPP::HashFilter(hash, new CryptoPP::HexEncoder(
                new CryptoPP::StringSink(digest), true)));
        node_id_ = utils::hexToBitset(digest);
    } catch (const std::exception &e) {
        qWarning() << "Exception during node_id creation: " << e.what();
        return {};
    }
    return digest;
}




