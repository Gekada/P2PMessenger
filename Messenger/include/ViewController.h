#ifndef SIMPLESERVERFRONT_VIEWCONTROLLER_H
#define SIMPLESERVERFRONT_VIEWCONTROLLER_H

#include <vector>
#include <memory>

#include <QObject>
#include <QtQmlIntegration>
#include <QtQml/QQmlContext>

#include "AuthorizationHandler.h"
#include "ViewTypes.h"

class ViewController: public QObject {
Q_OBJECT
QML_ELEMENT
QML_UNCREATABLE("C++ only object")
public:
    ViewController(QQmlContext *inputRootContext);
    void initializeHandlers(std::shared_ptr<QTcpSocket> connectionSocket);
    std::shared_ptr<networking::authorization::AuthorizationHandler> initializeAuthHandler(std::shared_ptr<QTcpSocket> connectionSocket);

signals:

void changeView(ViewTypes::ViewType type);

private:
    std::vector<std::shared_ptr<QObject>> viewHandlers;
    QQmlContext *rootContext;

    void initializeChatHandler(std::shared_ptr<QTcpSocket> connectionSocket);
};

#endif //SIMPLESERVERFRONT_VIEWCONTROLLER_H
