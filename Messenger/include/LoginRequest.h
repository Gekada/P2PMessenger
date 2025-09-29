//
// Created by L on 14.09.2024.
//

#ifndef SIMPLESERVERFRONT_LOGINREQUEST_H
#define SIMPLESERVERFRONT_LOGINREQUEST_H

#include "IRequest.h"

class LoginRequest: public IRequest{
public:

    LoginRequest(QString inputLogin, QString inputPassword);

    static std::unique_ptr<IRequest> fromJson(const QJsonObject &json);

    QJsonObject toJson() const override;

private:
    QString login;
    QString password;
};

#endif //SIMPLESERVERFRONT_LOGINREQUEST_H
