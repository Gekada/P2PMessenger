#include <iostream>
#include "node.h"
#include <QObject>
#include <QCoreApplication>


int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    kademlia::Node second(1338, true);

    kademlia::Node first;

    return QCoreApplication::exec();
}