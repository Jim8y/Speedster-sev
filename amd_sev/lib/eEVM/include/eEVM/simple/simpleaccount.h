// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "eEVM/account.h"
#include<string>
#include <nlohmann/json.hpp>
#include "eEVM/util.h"

namespace eevm {
    /**
     * Simple implementation of Account
     */
    class SimpleAccount : public Account {
    public:
        Address address = {};
        uint256_t balance = {};
        Pubkey pubkey = {};
        Prikey prikey = {};
        Code code = {};
        Nonce nonce = {};

    public:
        SimpleAccount() = default;

        SimpleAccount(const Address &a, const uint256_t &b, const Code &c) :
                address(a),
                balance(b),
                code(c),
                nonce(0) {}

        SimpleAccount(
                const Address &a, const uint256_t &b, const Code &c, Nonce n) :
                address(a),
                balance(b),
                code(c),
                nonce(n) {}

        virtual Address get_address() const override;

        virtual Pubkey get_pubkey() const override;

        void set_address(const Address &a);

        virtual uint256_t get_balance() const override;

        virtual void set_balance(const uint256_t &b) override;

        // Prikey get_prikey() const;
        // void set_prikey(const Prikey& pv);

        // Pubkey get_pubkey() const;
        // void set_pubkey(const Pubkey& pb);

        virtual Nonce get_nonce() const override;

        void set_nonce(Nonce n);

        virtual void increment_nonce() override;

        virtual Code get_code() const override;

        virtual void set_code(Code &&c) override;

        virtual bool has_code() override;

        bool operator==(const Account &) const;

        friend void to_json(nlohmann::json &, const SimpleAccount &);

        friend void from_json(const nlohmann::json &, SimpleAccount &);

        nlohmann::json to_json() {
            nlohmann::json j;
            j["address"] = to_hex_string(get_address());
            j["pubkey"] = to_hex_string(get_pubkey());
            return j;
        }
    };

    void to_json(nlohmann::json &, const SimpleAccount &);

    void from_json(const nlohmann::json &, SimpleAccount &);
} // namespace eevm
