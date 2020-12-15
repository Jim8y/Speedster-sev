//
// Created by compass on 9/15/19.
//

#ifndef LNTEE_HELPER_H
#define LNTEE_HELPER_H

#include <boost/endian/conversion.hpp>
#include <boost/functional/hash/hash.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <string>
#include <eEVM/bigint.h>
#include <eEVM/address.h>
#include <eEVM/common.h>

class Helper {

protected:
    std::vector<uint8_t> function_call;

public:
    Helper() {};

    ~Helper() {};

    /**
     * Show the command of the
     */
    virtual void showManu() = 0;

    auto funtion_call() {
        return function_call;
    }

    void clean() { this->function_call.clear(); };

    void append_argument(const uint256_t &arg);

    void append_argument(std::vector<uint8_t> &code, const uint256_t &arg);

    void append_argument(const uint8_t val);

    std::string strip(const std::string &s);

    std::vector<uint8_t> to_bytes(const std::string &_s);
};

#endif //LNTEE_HELPER_H
