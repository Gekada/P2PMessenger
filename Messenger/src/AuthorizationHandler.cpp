//
// Created by L on 18.08.2024.
//
#include <utility>

#include "AuthorizationHandler.h"


void networking::authorization::AuthorizationHandler::login(QString login, QString password) {

    LoginRequest request(login, password);
    QJsonDocument jsonDoc(request.toJson());
    QByteArray byteArray = jsonDoc.toJson(QJsonDocument::Compact);

//    QByteArray dataSize;
//    QDataStream dataStream(&dataSize, QIODevice::WriteOnly);
//    dataStream << byteArray.size();

    socket->write(byteArray);
    socket->waitForBytesWritten();

    byteArray.clear();
    socket->waitForReadyRead(300);

    byteArray.append(socket->readAll());

    jsonDoc = QJsonDocument::fromJson(byteArray);

    if (!jsonDoc.isObject()) {
        qWarning() << "Invalid JSON received!";
        return;
    }

    QJsonObject jsonObject = jsonDoc.object();

    std::unique_ptr<responseDto> response = responseDto::fromJson(jsonObject);

    if (response->status == 200) {
        emit authorizationSucceed();
    }
}

networking::authorization::AuthorizationHandler::AuthorizationHandler(std::shared_ptr<QTcpSocket> connectionSocket,
                                                                      QObject *parent) : socket(
        std::move(connectionSocket)), QObject(parent) {

}

