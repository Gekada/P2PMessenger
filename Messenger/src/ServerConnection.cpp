//
// Created by L on 19.08.2024.
//
#include "ServerConnection.h"

networking::ServerConnection::ServerConnection(QObject *parent) : QObject(parent), socket (std::make_shared<QTcpSocket>()) {

    socket->connectToHost(QHostAddress("127.0.0.1"),4000);

}

std::shared_ptr<QTcpSocket> networking::ServerConnection::getSocket() {
    return socket;
}

void networking::ServerConnection::closeConnection() {
    socket->close();
}

