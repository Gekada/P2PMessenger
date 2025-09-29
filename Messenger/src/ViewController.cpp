#include "ViewController.h"

ViewController::ViewController(QQmlContext *inputRootContext):rootContext(inputRootContext) {}

void ViewController::initializeHandlers(std::shared_ptr<QTcpSocket> connectionSocket) {
    initializeAuthHandler(connectionSocket);
    initializeChatHandler(connectionSocket);
}

std::shared_ptr<networking::authorization::AuthorizationHandler> ViewController::initializeAuthHandler(std::shared_ptr<QTcpSocket> connectionSocket) {
    auto *authHandler = new networking::authorization::AuthorizationHandler(std::move(connectionSocket));
    rootContext->setContextProperty("authorizationHandler", authHandler);

    connect(authHandler, &networking::authorization::AuthorizationHandler::authorizationSucceed, this, [this](){
        emit changeView(ViewTypes::ViewType::ChatView);
    });

    std::shared_ptr<networking::authorization::AuthorizationHandler> authHandlerPtr(authHandler);
    viewHandlers.push_back(authHandlerPtr);

    return authHandlerPtr;
}

void ViewController::initializeChatHandler(std::shared_ptr<QTcpSocket> connectionSocket) {

}
