#include <common/dispatcher.h>
#include <common/shared.h>
#include <common/message.h>
#include <common/eth_aes.h>
#include <common/eth_ecdsa.h>
#include <common/eth_ecp.h>
#include <common/wallet.h>
#include <common/thread_pool.h>
#include <common/dispatcher.h>

#include <eEVM/common.h>
#include <eEVM/bigint.h>
#include <eEVM/simple/simpleglobalstate.h>
#include <eEVM/debug.h>
#include <eEVM/transaction.h>
#include <nlohmann/json.hpp>
#include "mbedtls/config.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/ecdh.h"
#include "mbedtls/entropy.h"
#include "mbedtls/sha256.h"
#include "mbedtls/config.h"
#include "mbedtls/entropy.h"
#include "mbedtls/platform.h"
#include <mbedtls/aes.h>
#include <nlohmann/json.hpp>
#include "libf2f/router.h"
#include "TEE_contract.h"
#include <random>
#include <stdio.h>
#include <map>
#include <string.h>
#include <iostream>

#include <socket/config.h>


#ifndef ENCLAVE_H
#define ENCLAVE_H

//#include "balance.h"
using namespace libf2f;
using namespace std;

class Enclave {
private:
    ecall_dispatcher dispatcher;
    TEE_Contract contract;
    eevm::Address owner;

    libf2f::Router *router;

//    balance bal;
    ThreadPool *pool;
public:

    Enclave(libf2f::Router *r) : router(r) {}

    ~Enclave();

//    void set_router(libf2f::Router *r){ router = r; }
    void ecall_lntee_main(const char *contract_definition);

    int ecall_lntee_recv_transaction(const unsigned char *transaction);

    void ecall_lntee_send(const char *function_call);

    bool invoke_contract(const unsigned char *contract_addr,
                         eevm::Address origin,
                         const unsigned char *tx);

    int ecall_lntee_init_tee();

    // Process direct transaction pubkey: target pubkey, amt: amount to send
    void ecall_lntee_direct_send(string pubkey, int amt);

    int ecall_lntee_direct_recv(const char *tx);


    void process_message_from_host(const unsigned char *msg);

    bool process_contract_invoke(const unsigned char *contract_addr,
                                 eevm::Address origin,
                                 const unsigned char *tx);
};

#endif