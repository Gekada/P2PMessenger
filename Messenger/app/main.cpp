#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "AuthorizationHandler.h"
#include "ServerConnection.h"
#include "ViewController.h"

int main(int argc, char *argv[]) {

    QGuiApplication app(argc, argv);

    constexpr const char *app_name = "simpleServerFront";

    QGuiApplication::setApplicationName(app_name);
    QGuiApplication::setApplicationVersion("0.0.1");

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/P2PMessenger/ui/Main.qml"));
    QObject::connect(
            &engine, &QQmlApplicationEngine::objectCreated,
            &app, [url](QObject *obj, const QUrl &objUrl) {
                if (!obj && url == objUrl)
                    QCoreApplication::exit(-1);
            },
            Qt::QueuedConnection);
    engine.addImportPath(":/");

    std::unique_ptr<networking::ServerConnection> serverConnection = std::make_unique<networking::ServerConnection>();
    std::unique_ptr<ViewController> viewController = std::make_unique<ViewController>(engine.rootContext());
    //std::unique_ptr<networking::authorization::AuthorizationHandler> authorizationHandler = std::make_unique<networking::authorization::AuthorizationHandler>(serverConnection->getSocket());

    engine.rootContext()->setContextProperty("serverConnection", serverConnection.get());
    engine.rootContext()->setContextProperty("viewController", viewController.get());
    //engine.rootContext()->setContextProperty("authorizationHandler", authorizationHandler.get());

    auto authHandler = viewController->initializeAuthHandler(serverConnection->getSocket());

    engine.load(url);



    return QGuiApplication::exec();
}
