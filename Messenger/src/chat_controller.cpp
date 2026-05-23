#include "chat_controller.h"
#include "utils.h"
#include <QUuid>
#include <QDebug>

ChatController::ChatController(kademlia::Node *node, QObject *parent)
        : QObject(parent), node_(node) {}

void ChatController::setCurrentPeerId(const QString &peerId) {
    if (current_peer_id_ != peerId) {
        current_peer_id_ = peerId;
        emit currentPeerIdChanged();
    }
}

void ChatController::loadContacts() {
    emit requestMyId();
    emit requestContacts();
}

void ChatController::selectChat(const QString &peerId) {
    setCurrentPeerId(peerId);
    emit requestMessages(peerId);
}

void ChatController::sendMessage(const QString &text) {
    if (current_peer_id_.isEmpty() || text.trimmed().isEmpty()) return;

    std::bitset<128> receiver_id;
    try {
        receiver_id = kademlia::utils::hexToBitset(current_peer_id_.toStdString());
    } catch (...) { return; }

    emit requestSendMessage(receiver_id, text.toStdString(), receiver_id);

    //wait for DB confirmation (refactor)
    QMetaObject::invokeMethod(this, [this, current_peer = current_peer_id_]() {
        emit requestMessages(current_peer);
    }, Qt::QueuedConnection);
}

void ChatController::onContactsReceived(const QVariantList &contactsList) {
    contacts_ = contactsList;
    emit contactsChanged();
}

void ChatController::onMessagesReceived(const QVariantList &messagesList) {
    current_messages_ = messagesList;
    emit currentMessagesChanged();
}

void ChatController::onMyIdReceived(const QString& id) {
    if (my_peer_id_ != id) {
        my_peer_id_ = id;
        emit myPeerIdChanged();
    }
}