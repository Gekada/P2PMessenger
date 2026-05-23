#include "db_controller.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QUuid>

kademlia::data::DBController::DBController(const QString &db_filename, QObject *parent)
        : QObject(parent) {
    connection_name_ = QUuid::createUuid().toString();

    db_ = QSqlDatabase::addDatabase("QSQLITE", connection_name_);
    db_ .setDatabaseName(db_filename);

    if (!db_.open()) {
        qCritical() << "Failed to open database:" << db_filename << db_.lastError().text();
        return;
    }

    setupDatabase();
}

kademlia::data::DBController::~DBController() {
    db_.close();
    QSqlDatabase::removeDatabase(connection_name_);
}

void kademlia::data::DBController::setupDatabase() {
    QSqlQuery query(db_);

    query.exec("PRAGMA foreign_keys = ON;");

    QString createChatsTable = R"(
        CREATE TABLE IF NOT EXISTS "Contacts" (
            "id" INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
            "peerId" TEXT NOT NULL UNIQUE,
            "alias" TEXT,
            "lastKnownIp" TEXT,
            "lastKnownPort" INTEGER,
            "lastMessageDate" INTEGER
        );
    )";

    QString createMessagesTable = R"(
        CREATE TABLE IF NOT EXISTS "Messages" (
            "messageId" INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
            "chatId" INTEGER NOT NULL,
            "isOutgoing" INTEGER NOT NULL,
            "status" INTEGER NOT NULL DEFAULT 0,
            "data" TEXT NOT NULL,
            "timestamp" INTEGER NOT NULL,
            FOREIGN KEY("chatId") REFERENCES "Contacts"("id") ON DELETE CASCADE
        );
    )";

    QString createKademliaDataTable = R"(
        CREATE TABLE IF NOT EXISTS "KademliaData" (
          "id" INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
          "dataKey" TEXT NOT NULL,
          "value" TEXT NOT NULL,
          "dateCreated" INTEGER NOT NULL
        );
    )";

    if (!query.exec(createChatsTable)) qCritical() << "Contacts table error:" << query.lastError().text();
    if (!query.exec(createMessagesTable)) qCritical() << "Messages table error:" << query.lastError().text();
    if (!query.exec(createKademliaDataTable)) qCritical() << "KademliaData table error:" << query.lastError().text();
}

bool kademlia::data::DBController::storeValue(const std::string &key, const std::string &value) {
    QSqlQuery query(db_);
    query.prepare("INSERT OR IGNORE INTO KademliaData (dataKey, value, dateCreated) "
                  "VALUES (:dataKey, :value, :dateCreated)");

    query.bindValue(":dataKey", QString::fromStdString(key));
    query.bindValue(":value", QString::fromStdString(value));
    query.bindValue(":dateCreated", QDateTime::currentSecsSinceEpoch());

    if (!query.exec()) {
        qWarning() << "Failed to store value for key" << QString::fromStdString(key) << ":" << query.lastError().text();
        return false;
    }
    return true;
}

std::vector<std::string> kademlia::data::DBController::getValues(const std::string &key) {
    std::vector<std::string> values;
    QSqlQuery query(db_);
    query.prepare("SELECT value FROM KademliaData WHERE dataKey = :dataKey");
    query.bindValue(":dataKey", QString::fromStdString(key));

    if (query.exec()) {
        while (query.next()) {
            values.push_back(query.value(0).toString().toStdString());
        }
    }
    return values;
}

bool kademlia::data::DBController::hasValue(const std::string &key) {
    QSqlQuery query(db_);
    query.prepare("SELECT 1 FROM KademliaData WHERE dataKey = :key LIMIT 1");
    query.bindValue(":key", QString::fromStdString(key));
    return query.exec() && query.next();
}

bool kademlia::data::DBController::removeValue(const std::string &key) {
    QSqlQuery query(db_);
    query.prepare("DELETE FROM KademliaData WHERE dataKey = :key");
    query.bindValue(":key", QString::fromStdString(key));
    return query.exec();
}

std::vector<std::string> kademlia::data::DBController::getExpiredKeys(int64_t max_age_seconds) {
    std::vector<std::string> expired_keys;
    int64_t expiration_threshold = QDateTime::currentSecsSinceEpoch() - max_age_seconds;

    QSqlQuery query(db_);
    query.prepare("SELECT DISTINCT dataKey FROM KademliaData WHERE dateCreated < :threshold");
    query.bindValue(":threshold", expiration_threshold);

    if (query.exec()) {
        while (query.next()) {
            expired_keys.push_back(query.value(0).toString().toStdString());
        }
    }
    return expired_keys;
}

int kademlia::data::DBController::addOrUpdateContact(const Contact& c) {
    QSqlQuery query(db_);

    query.prepare("INSERT INTO Contacts (peerId, alias, lastKnownIp, lastKnownPort, lastMessageDate) "
                  "VALUES (:peerId, :alias, :ip, :port, :date) "
                  "ON CONFLICT(peerId) DO UPDATE SET "
                  "alias=excluded.alias, lastKnownIp=excluded.lastKnownIp, "
                  "lastKnownPort=excluded.lastKnownPort, lastMessageDate=excluded.lastMessageDate");

    query.bindValue(":peerId", QString::fromStdString(c.peerId));
    query.bindValue(":alias", QString::fromStdString(c.alias));
    query.bindValue(":ip", QString::fromStdString(c.lastKnownIp));
    query.bindValue(":port", c.lastKnownPort);
    query.bindValue(":date", static_cast<qint64>(c.lastMessageDate));

    if (query.exec()) {
        // ALWAYS do a direct SELECT after an SQLite Upsert to guarantee a valid ID
        QSqlQuery fetchQuery(db_);
        fetchQuery.prepare("SELECT id FROM Contacts WHERE peerId = :peerId");
        fetchQuery.bindValue(":peerId", QString::fromStdString(c.peerId));

        if (fetchQuery.exec() && fetchQuery.next()) {
            return fetchQuery.value(0).toInt();
        }
    }

    qWarning() << "Failed to add/update contact:" << query.lastError().text();
    return -1;
}

std::optional<kademlia::data::Contact> kademlia::data::DBController::getContactById(const std::string& peer_id) {
    QSqlQuery query(db_);
    // Fixed: peer_id -> peerId
    query.prepare("SELECT id, peerId, alias, lastKnownIp, lastKnownPort, lastMessageDate "
                  "FROM Contacts WHERE peerId = :peerId");
    query.bindValue(":peerId", QString::fromStdString(peer_id));

    if (query.exec() && query.next()) {
        Contact c;
        c.id = query.value(0).toInt();
        c.peerId = query.value(1).toString().toStdString();
        c.alias = query.value(2).toString().toStdString();
        c.lastKnownIp = query.value(3).toString().toStdString();
        c.lastKnownPort = query.value(4).toInt();
        c.lastMessageDate = query.value(5).toLongLong();
        return c;
    }
    return std::nullopt;
}

std::vector<kademlia::data::Contact> kademlia::data::DBController::getAllContacts() {
    std::vector<Contact> contacts;
    QSqlQuery query(db_);
    if (query.exec("SELECT id, peerId, alias, lastKnownIp, lastKnownPort, lastMessageDate FROM Contacts")) {
        while (query.next()) {
            Contact c;
            c.id = query.value(0).toInt();
            c.peerId = query.value(1).toString().toStdString();
            c.alias = query.value(2).toString().toStdString();
            c.lastKnownIp = query.value(3).toString().toStdString();
            c.lastKnownPort = query.value(4).toInt();
            c.lastMessageDate = query.value(5).toLongLong();
            contacts.push_back(c);
        }
    }
    return contacts;
}

bool kademlia::data::DBController::deleteContact(int id) {
    QSqlQuery query(db_);
    query.prepare("DELETE FROM Contacts WHERE id = :id");
    query.bindValue(":id", id);
    return query.exec();
}

int kademlia::data::DBController::addMessage(const Message& msg) {
    QSqlQuery query(db_);
    query.prepare("INSERT INTO Messages (chatId, isOutgoing, status, data, timestamp) "
                  "VALUES (:chatId, :isOut, :status, :data, :ts)");

    query.bindValue(":chatId", msg.chatId);
    query.bindValue(":isOut", msg.isOutgoing ? 1 : 0);
    query.bindValue(":status", msg.status);
    query.bindValue(":data", QString::fromStdString(msg.data));
    query.bindValue(":ts", static_cast<qint64>(msg.timestamp));

    if (query.exec()) {
        return query.lastInsertId().toInt();
    }
    qWarning() << "Failed to add message:" << query.lastError().text();
    return -1;
}

std::vector<kademlia::data::Message> kademlia::data::DBController::getChatMessages(int chatId, int limit, int offset) {
    std::vector<Message> messages;
    QSqlQuery query(db_);
    query.prepare("SELECT messageId, chatId, isOutgoing, status, data, timestamp "
                  "FROM Messages WHERE chatId = :chatId "
                  "ORDER BY timestamp ASC LIMIT :limit OFFSET :offset");

    query.bindValue(":chatId", chatId);
    query.bindValue(":limit", limit);
    query.bindValue(":offset", offset);

    if (query.exec()) {
        while (query.next()) {
            Message m;
            m.messageId = query.value(0).toInt();
            m.chatId = query.value(1).toInt();
            m.isOutgoing = query.value(2).toInt() != 0;
            m.status = query.value(3).toInt();
            m.data = query.value(4).toString().toStdString();
            m.timestamp = query.value(5).toLongLong();
            messages.push_back(m);
        }
    }
    return messages;
}

bool kademlia::data::DBController::updateMessageStatus(int message_id, int new_status) {
    QSqlQuery query(db_);
    query.prepare("UPDATE Messages SET status = :status WHERE messageId = :id");
    query.bindValue(":status", new_status);
    query.bindValue(":id", message_id);
    return query.exec();
}

bool kademlia::data::DBController::deleteMessage(int message_id) {
    QSqlQuery query(db_);
    query.prepare("DELETE FROM Messages WHERE messageId = :id");
    query.bindValue(":id", message_id);
    return query.exec();
}