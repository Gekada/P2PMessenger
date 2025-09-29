#ifndef SIMPLESERVERFRONT_RESPONSEDTO_H
#define SIMPLESERVERFRONT_RESPONSEDTO_H

#include <string>

#include <QJsonObject>

class responseDto{
public:

    qint64 status;
    QString message;

    responseDto(qint64 inputStatus, QString inputMessage);

    static std::unique_ptr<responseDto> fromJson(const QJsonObject &json);

    QJsonObject toJson() const;

};


#endif //SIMPLESERVERFRONT_RESPONSEDTO_H
