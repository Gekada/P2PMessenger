//
// Created by L on 19.08.2024.
//

#ifndef SIMPLESERVERFRONT_SERVERCONNECTION_H
#define SIMPLESERVERFRONT_SERVERCONNECTION_H

#include <QObject>
#include <QtQmlIntegration>
#include <QTcpSocket>

namespace networking{
    class ServerConnection: public QObject{
        Q_OBJECT
        QML_ELEMENT
        QML_UNCREATABLE("C++ only object")

    public:

        ServerConnection(QObject* parent = nullptr);


        std::shared_ptr<QTcpSocket> getSocket();

    public slots:

        void closeConnection();


    private:
        std::shared_ptr<QTcpSocket> socket;
    };

}

#endif //SIMPLESERVERFRONT_SERVERCONNECTION_H
