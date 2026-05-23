#include <iostream>
#include "node.h"
#include <QObject>
#include <QCoreApplication>
#include <QTimer>


int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    kademlia::Node second(1338);
    second.startNode(true);
//    kademlia::Node th(1339);
//    th.startNode(false,true);
//    kademlia::Node fr(1340, false, nullptr, true);
//    kademlia::Node ft(1341, false, nullptr, true);
//    kademlia::Node st(1342, false, nullptr, true);
//    kademlia::Node sevth(1343, false, nullptr, true);
//
//    kademlia::Node first;
//    first.startNode();
//
//    QTimer::singleShot(3000, [&th, &first]() {
//        th.sendStore(kademlia::utils::hexToBitset("DAD5CDB004705798EF097BDFB8CE968B"), "This should be in messages",
//                     kademlia::utils::hexToBitset("DAD5CDB004705798EF097BDFB8CE968B"));
//    });

//    QTimer::singleShot(3000, [&th, &first]() {
//        th.findValue(kademlia::utils::hexToBitset("DAD5CDB004705798EF097BDFB8CE968B"));
//    });
    return QCoreApplication::exec();
}