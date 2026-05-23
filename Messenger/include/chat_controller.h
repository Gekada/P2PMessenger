#ifndef P2PMESSENGER_CHAT_CONTROLLER_H
#define P2PMESSENGER_CHAT_CONTROLLER_H

#include <QObject>
#include <QString>
#include <QVariantList>

#include "node.h"

class ChatController : public QObject {
Q_OBJECT
    Q_PROPERTY(QString currentPeerId READ currentPeerId WRITE setCurrentPeerId NOTIFY currentPeerIdChanged)
    Q_PROPERTY(QVariantList contacts READ contacts NOTIFY contactsChanged)
    Q_PROPERTY(QVariantList currentMessages READ currentMessages NOTIFY currentMessagesChanged)
    Q_PROPERTY(QString myPeerId READ myPeerId NOTIFY myPeerIdChanged)

public:
    explicit ChatController(kademlia::Node *node, QObject *parent = nullptr);

    QString currentPeerId() const { return current_peer_id_; }
    void setCurrentPeerId(const QString &peerId);

    QVariantList contacts() const { return contacts_; }
    QVariantList currentMessages() const { return current_messages_; }

    QString myPeerId() const { return my_peer_id_; }

    Q_INVOKABLE void sendMessage(const QString &text);
    Q_INVOKABLE void loadContacts();
    Q_INVOKABLE void selectChat(const QString &peerId);

signals:
    void currentPeerIdChanged();
    void contactsChanged();
    void currentMessagesChanged();
    void myPeerIdChanged();

    void requestContacts();
    void requestMessages(const QString &peerId);
    void requestSendMessage(std::bitset<128> key, const std::string& value, std::bitset<128> receiver);

    void requestMyId();

public slots:
    void onContactsReceived(const QVariantList &contactsList);
    void onMessagesReceived(const QVariantList &messagesList);

    void onMyIdReceived(const QString& id);

private:
    kademlia::Node *node_;
    QString current_peer_id_;
    QVariantList contacts_;
    QVariantList current_messages_;
    QString my_peer_id_;
};

#endif //P2PMESSENGER_CHAT_CONTROLLER_H