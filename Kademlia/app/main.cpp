#include <iostream>
#include "node.h"
#include <QObject>
#include <QCoreApplication>


int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    kademlia::Node first;
    kademlia::Node second(1338);
    std::unique_ptr<QUdpSocket> udp_socket;
    udp_socket = std::make_unique<QUdpSocket>(nullptr);
    proto::Message test;
    test.set_type(proto::MessageType::FIND_NODE);
    udp_socket->writeDatagram(test.SerializeAsString().data(), test.SerializeAsString().size(), QHostAddress::LocalHost,
                              1337);
    udp_socket->writeDatagram(test.SerializeAsString().data(), test.SerializeAsString().size(), QHostAddress::LocalHost,
                              1338);
    return QCoreApplication::exec();
}