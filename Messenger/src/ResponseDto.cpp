#include "ResponseDto.h"


std::unique_ptr<responseDto> responseDto::fromJson(const QJsonObject &json) {
    std::unique_ptr<responseDto> responce = std::make_unique<responseDto>(json["status"].toInteger(),json["message"].toString());
    return responce;
}

QJsonObject responseDto::toJson() const {
    QJsonObject json;
    json["status"] = status;
    json["message"] = message;
    return json;
}

responseDto::responseDto(qint64 inputStatus, QString inputMessage): status(inputStatus), message(std::move(inputMessage)) {

}
