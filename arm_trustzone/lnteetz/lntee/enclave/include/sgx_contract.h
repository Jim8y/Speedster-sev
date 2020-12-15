//#include <common/message.h>
#include <eEVM/simple/simpleglobalstate.h>
#include <eEVM/bigint.h>
#include <eEVM/opcode.h>
#include <eEVM/processor.h>
#include <eEVM/common.h>
#include <eEVM/debug.h>
#include <random>
#include <stdio.h>
#include "message.h"

#ifndef SGX_Contract_HEADER_H
#define SGX_Contract_HEADER_H


class SGX_Contract {
public:
    std::vector<eevm::Address> users;
    Environment *env;
    const uint256_t total_supply = 1000;
    eevm::Address contract_address;

private:
    int rand_range(int exclusive_upper_bound);

    uint256_t get_random_uint256(size_t bytes = 32);

    std::vector<uint8_t> run_and_check_result(
            const eevm::Address &from, const eevm::Address &to, const eevm::Code &input);

    void append_argument(std::vector<uint8_t> &code, const uint256_t &arg);

    eevm::Address deploy_lnSGX_Contract();

public:
    SGX_Contract();

    void loadContract(nlohmann::json contract, eevm::Address *owner);

    std::string short_name(const eevm::Address &address);

    uint256_t get_total_supply();

    uint256_t get_balance(const eevm::Address &target_address);

    bool transfer(
            const eevm::Address &source_address,
            const eevm::Address &target_address,
            const uint256_t &amount);

    bool invoke(const eevm::Address &source_address, eevm::Code &function_call);

    void print_lntee_state(const std::string &heading);
};

#endif