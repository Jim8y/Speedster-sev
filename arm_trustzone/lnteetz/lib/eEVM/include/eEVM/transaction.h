// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "address.h"
#include "encoding.h"
#include <nlohmann/json.hpp>
#include <vector>

namespace eevm {
    namespace log {
        using Data = std::vector<uint8_t>;
        using Topic = uint256_t;
    }

    struct LogEntry {
        Address address;
        log::Data data;
        std::vector<log::Topic> topics;

        bool operator==(const LogEntry &that) const;

        friend void to_json(nlohmann::json &, const LogEntry &);

        friend void from_json(const nlohmann::json &, LogEntry &);
    };

    void to_json(nlohmann::json &, const LogEntry &);

    void from_json(const nlohmann::json &, LogEntry &);

    struct LogHandler {
        virtual ~LogHandler() = default;

        virtual void handle(LogEntry &&) = 0;
    };

    struct NullLogHandler : public LogHandler {
        virtual void handle(LogEntry &&) override {}
    };

    struct VectorLogHandler : public LogHandler {
        std::vector<LogEntry> logs;

        virtual ~VectorLogHandler() = default;

        virtual void handle(LogEntry &&e) override {
            logs.emplace_back(e);
        }
    };

    static vector<uint8_t> DEFAULT_VECTOR;

    /**
     * Ethereum transaction
     */
    struct Transaction {
        const Address origin;
        const Address target;
        const eevm::Code &script;
        uint8_t value = 0;
        const uint64_t gas_price;
        const uint64_t gas_limit;
        bytes32 r;
        bytes32 s;
        uint8_t v = 0;
        LogHandler &log_handler;
        std::vector<Address> selfdestruct_list;

        Transaction(
                const Address origin,
                LogHandler &lh,
                const Address to = NULL,
                const Code &input = DEFAULT_VECTOR,
                uint8_t value = 0,
                uint64_t gas_price = 0,
                uint64_t gas_limit = 0) :
                origin(origin),
                target(to),
                script(input),
                value(value),
                gas_price(gas_price),
                gas_limit(gas_limit),
                log_handler(lh) {}

        void text();

        void rlpEncode(bytes &out, bool withSig = true);

        void rlpDecode(const char *tx, size_t len);
    };

#if defined(__cplusplus)
    extern "C"
    {
#endif

    void rlp_item(const uint8_t *, int, bytes &); // __attribute__((deprecated));
    int form_transaction(
            int nonce,
            uint64_t request_id,
            uint8_t request_type,
            const uint8_t *request_data,
            size_t request_data_len,
            uint64_t resp_error,
            bytes resp_data,
            uint8_t *tx_output_bf,
            size_t *o_len,
            bool sign = true);

#if defined(__cplusplus)
    }
#endif
} // namespace eevm
