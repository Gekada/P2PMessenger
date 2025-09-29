#include <iostream>
#include "node.h"
#include "temp.h"
#include <QObject>
#include <QCoreApplication>


int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    kademlia::Node first;
    kademlia::Node second(1338);
    std::unique_ptr<QUdpSocket> udp_socket;
    udp_socket = std::make_unique<QUdpSocket>(nullptr);
    udp_socket->writeDatagram("aaaaaaaaaa", 10, QHostAddress::LocalHost, 1337);
    udp_socket->writeDatagram("aaaaaaaaaa", 10, QHostAddress::LocalHost, 1338);
    return QCoreApplication::exec();
}