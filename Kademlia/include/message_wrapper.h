//
// Created by L on 11.04.2026.
//

#ifndef P2PMESSENGER_MESSAGE_WRAPPER_H
#define P2PMESSENGER_MESSAGE_WRAPPER_H

#include <fmt/format.h>

#include <QDebug>

#include <string>
#include <stdexcept>

#include "winsock.h"

class MessageWrapper {
public:
    static constexpr short kHeaderSize = 2;
    static constexpr uint16_t kMaxBodySize = 0xFFFD;

    MessageWrapper() : has_header_(false) {}


    std::string& prepareBuffer(const size_t buffer_size) {
        has_header_ = false;
        if (buffer_size > kMaxBodySize) {
            throw std::invalid_argument("Message buffer size is too big");
        }

        message_.resize(buffer_size);
        return message_;
    }


    bool packMessage(const std::string& input_message) {
        if (input_message.size() > kMaxBodySize) {
            qDebug() << fmt::format("Cannot pack message with size {}\n", input_message.size());
            return false;
        }

        has_header_ = true;
        message_.resize(input_message.size() + kHeaderSize);

        *reinterpret_cast<uint16_t*>(message_.data()) = htons(input_message.size());

        std::copy(input_message.begin(), input_message.end(),
                  message_.begin() + kHeaderSize);

        return true;
    }

    [[nodiscard]] const std::string& getFullMessage() const { return message_; }

    [[nodiscard]] std::string getUnpackedMessage() const {
        if (!has_header_) {
            return message_;
        }
        // malformed message (has header but size is less than 2)
        if (message_.size() < kHeaderSize) {
            return {};
        }
        return {message_.begin() + kHeaderSize, message_.end()};
    }

    [[nodiscard]] uint16_t unpackHeader() const {
        if (message_.size() < 2) {
            return 0;
        }
        auto bodySize = ntohs(
                *reinterpret_cast<uint16_t*>(message_.substr(0, kHeaderSize).data()));
        return bodySize;
    }

    [[nodiscard]] uint16_t getFullSize() const { return message_.size(); }

private:
    std::string message_;
    bool has_header_;
};
#endif //P2PMESSENGER_MESSAGE_WRAPPER_H