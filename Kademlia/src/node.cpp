//
// Created by L on 03.08.2025.
//
#include <QNetworkDatagram>
#include <QTimer>
#include <QDateTime>

#include <nlohmann/json.hpp>

#include <cryptopp/shake.h>
#include <cryptopp/hex.h>
#include <cryptopp/filters.h>

#include <magic_enum/magic_enum.hpp>

#include <fstream>

#include "node.h"
#include "node_lookup_task.h"
#include "value_lookup_task.h"

const kademlia::Node::CallbackTable kademlia::Node::callbacks_{
        {proto::MessageType::PING,       [](const proto::Message &message, Node &node) {
            if (message.is_reply()) {
                if (!node.requests_map_.contains(message.rpc_id())) {
                    return;
                }
                qDebug() << "Got a response PING :\') port: " << node.kUdpPort;
                node.requests_map_.erase(message.rpc_id());
                if (!node.is_bootstrapped_) {
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
        {proto::MessageType::FIND_NODE,  [](const proto::Message &message, Node &node) {
            auto map_it = node.lookup_task_map_.find(message.rpc_id());
            if (message.is_reply() && map_it != node.lookup_task_map_.end()) {
                qDebug() << "Node FIND_NODE rpc response :\') port: " << node.kUdpPort;
                map_it->second->onResponseReceived(message);
                node.lookup_task_map_.erase(map_it);
            } else {
                qDebug() << "Node FIND_NODE rpc :\') port: " << node.kUdpPort;
                auto sender_node = node.r_table_.findNode(std::bitset<constants::kNodeIdSize>(message.from_user()));
                if (sender_node.has_value()) {
                    auto found_nodes = node.r_table_.findKNodes(std::bitset<constants::kNodeIdSize>(message.payload()));
                    std::string response = node.builder_.setSender(node.node_id_.to_string()).setReciever(
                            sender_node->node_id_.to_string()).setType(
                            proto::MessageType::FIND_NODE).setNodes(found_nodes).buildUnwrapped(message.rpc_id(),
                                                                                                true).SerializeAsString();
                    node.udp_socket_->writeDatagram(response.data(), response.size(),
                                                    sender_node->ip_address_,
                                                    sender_node->port_);
                } else {
                    qWarning() << "Got a message from an unknown node: " << magic_enum::enum_name(message.type())
                               << "port: " << node.kUdpPort;
                }
            }


        }},
        {proto::MessageType::STORE,      [](const proto::Message &message, Node &node) {
            if (message.is_reply()) {
                if (!node.requests_map_.contains(message.rpc_id())) {
                    return;
                }
                qDebug() << "Got a response STORE :\') port: " << node.kUdpPort;
                node.requests_map_.erase(message.rpc_id());
            } else {
                qDebug() << "Got a STORE rpc :\') port: " << node.kUdpPort;
                auto sender_node = node.r_table_.findNode(std::bitset<128>(message.from_user()));
                if (sender_node.has_value()) {
                    std::string response = node.builder_.setSender(node.node_id_.to_string()).setReciever(
                            sender_node->node_id_.to_string()).setType(
                            proto::MessageType::STORE).buildUnwrapped(message.rpc_id(), true).SerializeAsString();

                    proto::DelayedChatMessage delayed_message;
                    // if the first message is not delayed
                    if (!delayed_message.ParseFromString(message.data()[0].value())) {
                        for (const auto data_record: message.data()) {
                            auto contact = node.db_controller_->getContactById(
                                    utils::bitsetToHex(sender_node->node_id_));
                            int contact_id;
                            if (!contact.has_value()) {
                                data::Contact new_contact;
                                new_contact.alias = utils::bitsetToHex(sender_node->node_id_);
                                new_contact.peerId = utils::bitsetToHex(sender_node->node_id_);
                                contact_id = node.db_controller_->addOrUpdateContact(new_contact);
                            } else {
                                contact_id = contact.value().id;
                            }
                            if (contact_id >= 0) {
                                data::Message new_message;
                                new_message.chatId = contact_id;
                                new_message.timestamp = QDateTime::currentSecsSinceEpoch();
                                new_message.data = data_record.value();
                                if (node.db_controller_->addMessage(new_message)>=0){
                                    emit node.receivedMessage(data_record);
                                }
                                else{
                                    qWarning() << "Failed to save incoming message, port: " << node.kUdpPort;
                                }
                            }

                        }
//                        if (utils::hexToBitset(message.data()[0].key()) == node.node_id_) {
//                            emit node.receivedMessage(message.data()[0]);
//                        }
                    } else {
                        for (const auto data_record: message.data()) {
                            if (!node.db_controller_->storeValue(data_record.key(), data_record.value())) {
                                qWarning() << "Failed to save data, port: " << node.kUdpPort;
                                // for now just return, later add transactions
                                return;
                            }
                        }

                    }
                    node.udp_socket_->writeDatagram(response.data(), response.size(),
                                                    sender_node->ip_address_,
                                                    sender_node->port_);
                } else {
                    qWarning() << "Got a message from an unknown node: " << magic_enum::enum_name(message.type())
                               << "port: " << node.kUdpPort;
                }
            }
        }},
        {proto::MessageType::FIND_VALUE, [](const proto::Message &message, Node &node) {
            auto map_it = node.lookup_task_map_.find(message.rpc_id());
            if (message.is_reply() && map_it != node.lookup_task_map_.end()) {
                qDebug() << "Node FIND_VALUE rpc response :\') port: " << node.kUdpPort;
                map_it->second->onResponseReceived(message);
                node.lookup_task_map_.erase(map_it);
            } else {
                qDebug() << "Node FIND_VALUE rpc :\') port: " << node.kUdpPort;
                auto sender_node = node.r_table_.findNode(std::bitset<constants::kNodeIdSize>(message.from_user()));
                auto data_key = message.payload();
                if (sender_node.has_value()) {
                    auto data_values = node.db_controller_->getValues(data_key);
                    if (!data_values.empty()) {
                        std::vector<proto::DataInfo> messages_to_send;
                        for (const auto &record: data_values) {
                            proto::DataInfo proto_record;
                            proto_record.set_key(data_key);
                            proto_record.set_value(record);
                            messages_to_send.push_back(proto_record);
                        }
                        std::string response = node.builder_.setSender(node.node_id_.to_string()).setReciever(
                                sender_node->node_id_.to_string()).setType(
                                proto::MessageType::FIND_VALUE).setData(messages_to_send).buildUnwrapped(
                                message.rpc_id(),
                                true).SerializeAsString();
                        node.udp_socket_->writeDatagram(response.data(), response.size(),
                                                        sender_node->ip_address_,
                                                        sender_node->port_);
                    } else {
                        qDebug() << "Coulnd't find value for key: " << data_key << " port: " << node.kUdpPort;
                        auto found_nodes = node.r_table_.findKNodes(std::bitset<constants::kNodeIdSize>(data_key));
                        std::string response = node.builder_.setSender(node.node_id_.to_string()).setReciever(
                                sender_node->node_id_.to_string()).setType(
                                proto::MessageType::FIND_VALUE).setNodes(found_nodes).buildUnwrapped(message.rpc_id(),
                                                                                                     true).SerializeAsString();
                        node.udp_socket_->writeDatagram(response.data(), response.size(),
                                                        sender_node->ip_address_,
                                                        sender_node->port_);
                    }
                } else {
                    qWarning() << "Got a message from an unknown node: " << magic_enum::enum_name(message.type())
                               << "port: " << node.kUdpPort;
                }
            }
        }}
};

kademlia::Node::Node(QObject *parent) : Node(kademlia::constants::kDefaultUdpPort) {}

// TODO: Don't forget to delete from here too
kademlia::Node::Node(uint16_t input_udp_port, QObject *parent) : QObject(parent),
                                                                 kUdpPort(
                                                                         input_udp_port),
                                                                 r_table_(node_id_) {}

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
    if (!config_input.is_open()) {
        qWarning() << "Couldn't open storage file";
        return;
    }

    nlohmann::json data = nlohmann::json::parse(config_input);

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
    QTimer::singleShot(2500, this, [this, id, type = message.type()] {
        if (requests_map_.erase(id)) {
            qDebug() << "Request timed out: " << magic_enum::enum_name(type);
        }
    });
}

void kademlia::Node::bootstrap() {

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

    QObject::connect(this, &Node::finishedBootstrap, [this]() {
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
        r_table_.storeNode({node.port(), QHostAddress(QString::fromStdString(node.ip())),
                            std::bitset<constants::kNodeIdSize>(node.node_id())});
    }
}

const std::bitset<kademlia::constants::kNodeIdSize> &kademlia::Node::getId() {
    return node_id_;
}

void kademlia::Node::sendStore(std::bitset<constants::kNodeIdSize> key, const std::string &value,
                               std::bitset<constants::kNodeIdSize> receiver_id) {
    proto::DelayedChatMessage delayed_message;
    //if it's normal message
    if (!delayed_message.ParseFromString(value)){
        std::string peer_hex = utils::bitsetToHex(receiver_id);
        auto contact = db_controller_->getContactById(peer_hex);
        int contact_id;

        if (!contact.has_value()) {
            data::Contact new_contact;
            new_contact.alias = peer_hex;
            new_contact.peerId = peer_hex;
            contact_id = db_controller_->addOrUpdateContact(new_contact);
        } else {
            contact_id = contact.value().id;
        }

        if (contact_id >= 0) {
            data::Message msg;
            msg.chatId = contact_id;
            msg.isOutgoing = true;
            msg.timestamp = QDateTime::currentSecsSinceEpoch();
            msg.data = value;
            db_controller_->addMessage(msg);
        }
    }
    proto::DataInfo data_entry;
    data_entry.set_key(key.to_string());
    data_entry.set_value(value);
    auto message = builder_.setSender(node_id_.to_string()).setReciever(receiver_id.to_string()).setType(
            proto::MessageType::STORE).setData({data_entry}).buildUnwrapped();

    auto receiver_node = r_table_.findNode(receiver_id);

    if (receiver_node.has_value()) {
        sendRequest(message, receiver_node->ip_address_, receiver_node->port_);
    } else {
        qWarning() << "Couldn't find the node for STORE rpc, port: " << kUdpPort;
    }
}

void kademlia::Node::findValue(std::bitset<constants::kNodeIdSize> target_id) {
    std::shared_ptr<ValueLookupTask> task = std::make_shared<ValueLookupTask>(r_table_, target_id, node_id_);
    std::weak_ptr<ValueLookupTask> weak_task = task;
    QObject::connect(task.get(), &ValueLookupTask::valueSearchSucceeded, this,
                     [this](std::vector<proto::DataInfo> found_values) {
                         qDebug() << "Finished value lookup, port: " << kUdpPort;
                         for (auto data: found_values) {
                             qDebug() << "Found value: " << data.value();
                         }
                     });
    QObject::connect(task.get(), &ValueLookupTask::valueSearchFailed, this, [this](std::vector<NodeEntry> found_nodes) {
        qDebug() << "Failed value search, port: " << kUdpPort;
    });
    QObject::connect(task.get(), &ValueLookupTask::requestCreated,
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

void kademlia::Node::startNode(bool is_bootstrap, bool is_fake) {
    db_controller_ = std::make_unique<data::DBController>(QString("kademlia_node_%1.db").arg(kUdpPort), this);
    initSocket();
    if (!is_fake && !is_bootstrap) {
        initNodeId();
    } else {
        node_id_ = utils::hexToBitset(generateNodeId());
    }
    r_table_ = RoutingTable(node_id_);

    if (!is_bootstrap) {
        bootstrap();
    }
}

void kademlia::Node::fetchContactsFromDb() {
    QVariantList list;
    auto contacts = db_controller_->getAllContacts();
    for (const auto& c : contacts) {
        QVariantMap map;
        map["id"] = c.id;
        map["peerId"] = QString::fromStdString(c.peerId);
        map["alias"] = QString::fromStdString(c.alias);
        list.append(map);
    }
    emit contactsFetched(list);
}

void kademlia::Node::fetchMessagesFromDb(const QString& peerId) {
    QVariantList list;
    auto contact = db_controller_->getContactById(peerId.toStdString());
    if (contact.has_value()) {
        auto msgs = db_controller_->getChatMessages(contact.value().id, 100); // Get last 100 msgs
        for (const auto& m : msgs) {
            QVariantMap map;
            map["isMine"] = m.isOutgoing;
            map["message"] = QString::fromStdString(m.data);
            list.append(map);
        }
    }
    emit messagesFetched(list);
}

void kademlia::Node::fetchMyId() {
    emit myIdFetched(QString::fromStdString(utils::bitsetToHex(node_id_)));
}
