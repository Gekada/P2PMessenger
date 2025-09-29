//
// Created by L on 13.09.2024.
//

#ifndef SIMPLESERVERFRONT_IREQUEST_H
#define SIMPLESERVERFRONT_IREQUEST_H

#include <QJsonObject>

class IRequest{
public:

    static std::unique_ptr<IRequest> fromJson(const QJsonObject &json);

    [[nodiscard]] virtual QJsonObject toJson() const = 0;

protected:

    QString type;
};


#endif //SIMPLESERVERFRONT_IREQUEST_H
