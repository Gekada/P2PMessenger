//
// Created by L on 15.09.2024.
//
#include "LoginRequest.h"

#include <utility>

LoginRequest::LoginRequest(QString inputLogin, QString inputPassword): login(std::move(inputLogin)), password(std::move(inputPassword)) {
    type = QString("login");
}

std::unique_ptr<IRequest> LoginRequest::fromJson(const QJsonObject &json) {
    std::unique_ptr<IRequest> request = std::make_unique<LoginRequest>(json["login"].toString(),json["password"].toString());
    return request;
}

QJsonObject LoginRequest::toJson() const {
    QJsonObject json;
    json["type"] = type;
    json["login"] = login;
    json["password"] = password;
    return json;
}

