#ifndef SIMPLESERVERFRONT_AUTHORIZATIONHANDLER_H
#define SIMPLESERVERFRONT_AUTHORIZATIONHANDLER_H

#include <QObject>
#include <QtQmlIntegration>
#include <QTcpSocket>
#include <QJsonDocument>

#include "LoginRequest.h"
#include "ResponseDto.h"

namespace networking::authorization{

    class AuthorizationHandler: public QObject{
        Q_OBJECT
        QML_ELEMENT
        QML_UNCREATABLE("C++ only object")

    public:

        explicit AuthorizationHandler(std::shared_ptr<QTcpSocket> connectionSocket, QObject* parent = nullptr);

        Q_INVOKABLE void login(QString login, QString password);

        signals:

        void authorizationSucceed();

    private:
        std::shared_ptr<QTcpSocket> socket;
    };

}

#endif //SIMPLESERVERFRONT_AUTHORIZATIONHANDLER_H
