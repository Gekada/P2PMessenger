//
// Created by L on 26.04.2026.
//
#ifndef P2PMESSENGER_DB_CONTROLLER_H
#define P2PMESSENGER_DB_CONTROLLER_H

#include <QObject>
#include <QSqlDatabase>
#include <QString>
#include <string>
#include <optional>
#include <vector>

namespace kademlia::data {
// due to a fucking lack of time (god give me strength to finish this in time) I'm putting this class in kademlia
// but later this should me moved perhaps to a different lib
    struct Contact {
        int id = -1;
        std::string peerId;
        std::string alias;
        std::string lastKnownIp;
        int lastKnownPort = 0;
        int64_t lastMessageDate = 0;
    };

    struct Message {
        int messageId = -1;
        int chatId = -1;
        bool isOutgoing = false;
        int status = 0;
        std::string data;
        int64_t timestamp = 0;
    };

    class DBController : public QObject {
    Q_OBJECT

    public:
        explicit DBController(const QString &db_filename, QObject *parent = nullptr);

        ~DBController();

        // --- Kademlia DHT Storage ---
        bool storeValue(const std::string &key, const std::string &value);

        std::vector<std::string> getValues(const std::string &key);
        bool hasValue(const std::string &key);

        bool removeValue(const std::string &key);

        std::vector<std::string> getExpiredKeys(int64_t max_age_seconds);

        // --- Contacts CRUD ---
        int addOrUpdateContact(const Contact &contact);

        std::optional<Contact> getContactById(const std::string &peer_id);

        std::vector<Contact> getAllContacts();

        bool deleteContact(int id);

        // --- Messages CRUD ---
        int addMessage(const Message &msg);

        std::vector<Message> getChatMessages(int chatId, int limit = 50, int offset = 0);

        bool updateMessageStatus(int message_id, int new_status);

        bool deleteMessage(int message_id);

    private:
        void setupDatabase();

        QSqlDatabase db_;
        QString connection_name_;
    };
}
#endif //P2PMESSENGER_DB_CONTROLLER_H