//
// Created by L on 11.04.2026.
//

#ifndef P2PMESSENGER_MESSAGE_BUILDER_H
#define P2PMESSENGER_MESSAGE_BUILDER_H
#include <fmt/chrono.h>

#include <optional>

#include "message.pb.h"
#include "message_wrapper.h"
#include "utils.h"

class MessageBuilder {
public:

    MessageBuilder& setType(const proto::MessageType type) {
        protoMessage_.set_type(type);
        return *this;
    }

    proto::Message buildUnwrapped() {
        const auto now = std::chrono::system_clock::now();
        const int64_t timestamp =
                std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch())
                        .count();
        protoMessage_.set_timestamp(timestamp);
        proto::Message output;
        swap(protoMessage_, output);
        return output;
    }

    std::optional<MessageWrapper> build() {
        const auto now = std::chrono::system_clock::now();
        const int64_t timestamp =
                std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch())
                        .count();
        protoMessage_.set_timestamp(timestamp);
        std::string serialized = protoMessage_.SerializeAsString();
        MessageWrapper output;
        if (output.packMessage(serialized)) {
            protoMessage_.Clear();
            return {std::move(output)};
        }
        fmt::println("Failed to build message {}, size is too big",
                     protoMessage_.type());
        return std::nullopt;
    }

private:
    proto::Message protoMessage_;
};
#endif //P2PMESSENGER_MESSAGE_BUILDER_H
