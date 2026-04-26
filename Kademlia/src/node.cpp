//
// Created by L on 03.08.2025.
//
#include <QNetworkDatagram>
#include <QTimer>

#include <nlohmann/json.hpp>

#include <cryptopp/shake.h>
#include <cryptopp/hex.h>
#include <cryptopp/filters.h>

#include <magic_enum/magic_enum.hpp>

#include <fstream>

#include "node.h"
#include "node_lookup_task.h"

const kademlia::Node::CallbackTable kademlia::Node::callbacks_{
        {proto::MessageType::PING,      [](const proto::Message &message, Node &node) {
            if (message.is_reply()) {
                if (!node.requests_map_.contains(message.rpc_id())) {
                    return;
                }
                qDebug() << "Got a response PING :\') port: " << node.kUdpPort;
                node.requests_map_.erase(message.rpc_id());
                if (!node.is_bootstrapped_){
                    node.is_bootstrapped_ = true;
                    emit node.finishedBootstrap();
                }
            } else {
                qDebug() << "Got a PING rpc :\') port: " << node.kUdpPort;
                auto sender_node = node.r_table_.findNode(std::bitset<128>(message.from_user()));
                if (sender_node.has_value()) {
                    std::string response = node.builder_.setSender(node.node_id_.to_string()).setReciever(
                            sender_node->node_id_.to_string()).setType(
                            proto::MessageType::PING).buildUnwrapped(message.rpc_id(), true).SerializeAsString();
                    node.udp_socket_->writeDatagram(response.data(), response.size(),
                                                    sender_node->ip_address_,
                                                    sender_node->port_);

                } else {
                    qWarning() << "Got a message from an unknown node: " << magic_enum::enum_name(message.type())
                               << "port: " << node.kUdpPort;
                }
            }
        }},
        {proto::MessageType::FIND_NODE, [](const proto::Message &message, Node &node) {
            auto map_it = node.lookup_task_map_.find(message.rpc_id());
            if (message.is_reply() && map_it != node.lookup_task_map_.end()) {
                qDebug() << "Node FIND_NODE rpc response :\') port: " << node.kUdpPort;
                map_it->second->onResponseReceived(message);
                node.lookup_task_map_.erase(map_it);
            } else {
                qDebug() << "Node FIND_NODE rpc :\') port: " << node.kUdpPort;
                auto sender_node = node.r_table_.findNode(std::bitset<constants::kNodeIdSize>(message.from_user()));
                auto found_nodes = node.r_table_.findKNodes(std::bitset<constants::kNodeIdSize>(message.payload()));
                std::string response = node.builder_.setSender(node.node_id_.to_string()).setReciever(
                        sender_node->node_id_.to_string()).setType(
                        proto::MessageType::FIND_NODE).setNodes(found_nodes).buildUnwrapped(message.rpc_id(),
                                                                                            true).SerializeAsString();
                node.udp_socket_->writeDatagram(response.data(), response.size(),
                                                sender_node->ip_address_,
                                                sender_node->port_);
            }


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
    r_table_ = RoutingTable(node_id_);
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

void kademlia::Node::onReceive() {
    while (udp_socket_->hasPendingDatagrams()) {
        QNetworkDatagram datagram = udp_socket_->receiveDatagram();
        QByteArray data = datagram.data();

        proto::Message received_message;
        if (received_message.ParseFromArray(data.data(), data.size())) {
            // storing each node we got a message from
            r_table_.storeNode({datagram.senderPort(), datagram.senderAddress(),
                                std::bitset<constants::kNodeIdSize>(received_message.from_user())});
            if (received_message.type() == proto::MessageType::FIND_NODE ||
                received_message.type() == proto::MessageType::FIND_VALUE) {
                registerNestedNodes(received_message);
            }
            processMessage(received_message);
        } else {
            qWarning("Failed to parse datagram into protobuf message");
        }
    }
}

void kademlia::Node::processMessage(const proto::Message &input_message) try {
    auto callback = callbacks_.find(input_message.type());
    if (callback == callbacks_.end()) {
        qWarning() << "Invalid message type: " << magic_enum::enum_name(input_message.type());
        return;
    }
    callback->second(input_message, *this);
} catch (const std::exception &ex) {
    qWarning() << "Caught an exception: " << ex.what();
}

bool kademlia::Node::isNewConnection() {
    std::ifstream config_input(constants::confPath);

    if (!config_input.is_open()) {
        qWarning() << "Couldn't open storage file";
        return false;
    }

    nlohmann::json data = nlohmann::json::parse(config_input);

    if (data.find("node_id") == data.end()) {
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
        if (isNewConnection()) {
            std::string digest = generateNodeId();
            data["node_id"] = digest;
            std::ofstream config_output(constants::confPath);
            config_output << data;
            node_id_ = utils::hexToBitset(digest);
        } else {
            node_id_ = utils::hexToBitset(data["node_id"]);
        }
    } catch (const std::exception &e) {
        qWarning() << "Exception during node_id init: " << e.what();
        return;
    }
}

std::string kademlia::Node::generateNodeId() {
    //Generating id by ip:port for consistency in debugging
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

void kademlia::Node::pushRequestToMap(const proto::Message &message) {
    requests_map_.insert({message.rpc_id(), message.type()});
    std::string id = message.rpc_id();

    // 2 sec until the message is deleted from queue and be ignored
    QTimer::singleShot(2000, this, [this, id, type = message.type()] {
        if (requests_map_.erase(id)) {
            qDebug() << "Request timed out: " << magic_enum::enum_name(type);
        }
    });
}

void kademlia::Node::bootstrap() {
    initNodeId();

    std::ifstream config_input(constants::confPath);

    if (!config_input.is_open()) {
        qWarning() << "Couldn't open storage file";
        return;
    }
    nlohmann::json data = nlohmann::json::parse(config_input);

    proto::Message proto_message = builder_.setType(proto::MessageType::PING).setSender(
            node_id_.to_string()).buildUnwrapped();
    std::string bootstrap_node_ip = data["bootstrap_node_ip"];
    int bootstrap_node_port = std::stoi(std::string(data["bootstrap_node_port"]));

    sendRequest(proto_message, QHostAddress(bootstrap_node_ip.data()), bootstrap_node_port);

    QObject::connect(this, &Node::finishedBootstrap, [this](){
        findNode(node_id_);
    });

}

void kademlia::Node::findNode(std::bitset<constants::kNodeIdSize> target_id) {
    std::shared_ptr<NodeLookupTask> task = std::make_shared<NodeLookupTask>(r_table_, target_id, node_id_);
    std::weak_ptr<NodeLookupTask> weak_task = task;
    QObject::connect(task.get(), &NodeLookupTask::nodeSearchFinished, this, [this](std::vector<NodeEntry> found_nodes) {
        qDebug() << "Finished node lookup, port: " << kUdpPort;
        for (auto node: found_nodes) {
            qDebug() << "Found node: " << node.node_id_.to_string();
        }
    });
    QObject::connect(task.get(), &NodeLookupTask::requestCreated,
                     [this, weak_task](const proto::Message &request, const QHostAddress &receiver, int port) {
                         if (auto task_shared = weak_task.lock()) {
                             lookup_task_map_.insert({request.rpc_id(), task_shared});
                             sendRequest(request, receiver, port);
                             QTimer::singleShot(2000, this,
                                                [this, id = request.rpc_id(), type = request.type()] { // Removed &task_shared
                                                    auto it = lookup_task_map_.find(id);
                                                    if (it != lookup_task_map_.end()) {
                                                        auto task_ptr = it->second;
                                                        lookup_task_map_.erase(it);
                                                        task_ptr->requestTimedOut();
                                                        qDebug() << "Request timed out: "
                                                                 << magic_enum::enum_name(type);
                                                    }
                                                });
                         }
                     });
    task->start();
}

void kademlia::Node::sendRequest(const proto::Message &proto_message, const QHostAddress &receiver, int port) {
    std::string message = proto_message.SerializeAsString();

    if (proto_message.type() != proto::MessageType::FIND_NODE &&
        proto_message.type() != proto::MessageType::FIND_VALUE) {
        pushRequestToMap(proto_message);
    }
    udp_socket_->writeDatagram(message.data(), message.size(), receiver, port);
}

void kademlia::Node::registerNestedNodes(const proto::Message &proto_message) {
    if (proto_message.type() != proto::MessageType::FIND_NODE &&
        proto_message.type() != proto::MessageType::FIND_VALUE) {
        qWarning() << "Wrong message type to register nested nodes: " << magic_enum::enum_name(proto_message.type());
        return;
    }
    for (const auto &node: proto_message.closest_nodes()) {
        r_table_.storeNode({node.port(), QHostAddress(node.node_id().data()),
                            std::bitset<constants::kNodeIdSize>(node.node_id())});
    }
}

const std::bitset<kademlia::constants::kNodeIdSize> &kademlia::Node::getId() {
    return node_id_;
}

