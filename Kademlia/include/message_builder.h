//
// Created by L on 11.04.2026.
//

#ifndef P2PMESSENGER_MESSAGE_BUILDER_H
#define P2PMESSENGER_MESSAGE_BUILDER_H
#include <fmt/chrono.h>

#include <optional>

#include <cryptopp/osrng.h>
#include <cryptopp/secblock.h>
#include <cryptopp/hex.h>
#include <cryptopp/filters.h>

#include "message.pb.h"
#include "message_wrapper.h"
#include "utils.h"

class MessageBuilder {
public:

    MessageBuilder& setType(const proto::MessageType type) {
        protoMessage_.set_type(type);
        return *this;
    }

    MessageBuilder& setReciever(const std::string& node_id) {
        protoMessage_.set_to_user(node_id);
        return *this;
    }

    MessageBuilder& setSender(const std::string& node_id) {
        protoMessage_.set_from_user(node_id);
        return *this;
    }

    MessageBuilder& setPayload(const std::string& payload) {
        protoMessage_.set_payload(payload);
        return *this;
    }

    proto::Message buildUnwrapped(std::string input_rpc_id = ""){
        const auto now = std::chrono::system_clock::now();
        const int64_t timestamp =
                std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch())
                        .count();
        protoMessage_.set_timestamp(timestamp);

        if (input_rpc_id.empty()){
            CryptoPP::AutoSeededRandomPool prng;

            const size_t rpcIdLength = 4;
            CryptoPP::SecByteBlock rpc_id(rpcIdLength);
            prng.GenerateBlock(rpc_id, rpc_id.size());
            CryptoPP::StringSource ss(rpc_id.data(), rpc_id.size(), true);
            protoMessage_.set_rpc_id(rpc_id.data(), rpc_id.size());
        }
        else{
            protoMessage_.set_rpc_id(input_rpc_id.data(), input_rpc_id.size());
        }

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
