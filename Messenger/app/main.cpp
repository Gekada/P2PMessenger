#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QThread>

#include "node.h"
#include "chat_controller.h"

int main(int argc, char *argv[]) {

    QGuiApplication app(argc, argv);

    constexpr const char *app_name = "p2pMessenger";

    QGuiApplication::setApplicationName(app_name);
    QGuiApplication::setApplicationVersion("0.0.1");

    QThread *kademliaThread = new QThread();

    kademlia::Node *myNode = new kademlia::Node(1337);

    myNode->moveToThread(kademliaThread);

    QObject::connect(kademliaThread, &QThread::started, myNode, [myNode]() {
        myNode->startNode();
    });

    QObject::connect(&app, &QCoreApplication::aboutToQuit, kademliaThread, &QThread::quit);
    QObject::connect(kademliaThread, &QThread::finished, myNode, &QObject::deleteLater);
    QObject::connect(kademliaThread, &QThread::finished, kademliaThread, &QObject::deleteLater);

    kademliaThread->start();

    QQmlApplicationEngine engine;

    ChatController chatController(myNode);

    QObject::connect(&chatController, &ChatController::requestContacts, myNode, &kademlia::Node::fetchContactsFromDb);
    QObject::connect(&chatController, &ChatController::requestMessages, myNode, &kademlia::Node::fetchMessagesFromDb);
    QObject::connect(&chatController, &ChatController::requestSendMessage, myNode, &kademlia::Node::sendStore);

    QObject::connect(myNode, &kademlia::Node::contactsFetched, &chatController, &ChatController::onContactsReceived);
    QObject::connect(myNode, &kademlia::Node::messagesFetched, &chatController, &ChatController::onMessagesReceived);

    QObject::connect(&chatController, &ChatController::requestMyId, myNode, &kademlia::Node::fetchMyId);
    QObject::connect(myNode, &kademlia::Node::myIdFetched, &chatController, &ChatController::onMyIdReceived);

    QObject::connect(myNode, &kademlia::Node::receivedMessage, &chatController, [&chatController]() {
        chatController.loadContacts();
        if (!chatController.currentPeerId().isEmpty()) {
            chatController.selectChat(chatController.currentPeerId());
        }
    });

    engine.rootContext()->setContextProperty("chatController", &chatController);

    const QUrl url(QStringLiteral("qrc:/P2PMessenger/ui/Main.qml"));
    QObject::connect(
            &engine, &QQmlApplicationEngine::objectCreated,
            &app, [url](QObject *obj, const QUrl &objUrl) {
                if (!obj && url == objUrl)
                    QCoreApplication::exit(-1);
            },
            Qt::QueuedConnection);
    engine.addImportPath(":/");

    engine.load(url);


    return QGuiApplication::exec();
}
