#include "lntee_t.h"
//#include "eEVM/bigint.h"
//#include "eEVM/common.h"
//#include "eEVM/simple/simpleglobalstate.h"
//#include "eEVM/transaction.h"
#include "mbedtls/sha256.h"
#include "mbedtls/platform.h"
#include <random>
#include <stdio.h>
#include <map>
//#include <eEVM/debug.h>
#include <openenclave/enclave.h>



extern "C"
{
//#include "keccak/KeccakHash.h"
}
//#include <iostream>
//#include <fmt/format_header_only.h>
//#include <iomanip>
//#include <limits>
//#include <nlohmann/json.hpp>
//#include <sstream>
//#include <intx/intx.hpp>
//#include <vector>
//#include <array>
//#include<string>
//using std::vector;
//using std::invalid_argument;
//using std::string;
//using std::array;
//typedef std::vector<uint8_t> BYTE;
#include <openenclave/edger8r/enclave.h>
#include <openenclave/enclave.h>
#include <openenclave/internal/globals.h> // for __oe_get_enclave_base()
#include <openenclave/internal/tests.h>
#include <openenclave/internal/thread.h>
#include <openenclave/internal/trace.h>
#include <stdio.h>
#include <mutex>
#include <system_error>
#include <string>
#define MAX_OPT_MESSAGE_LEN 128
#define IV_SIZE 16
#define SIGNATURE_LEN 32
//using uint256_tt = intx::uint256;
//using uint512_tt = intx::uint512;

typedef struct _sealed_data_t {
    size_t total_size;
    unsigned char signature[SIGNATURE_LEN];
    unsigned char opt_msg[MAX_OPT_MESSAGE_LEN];
    unsigned char iv[IV_SIZE];
    size_t key_info_size;
    size_t original_data_size;
    size_t encrypted_data_len;
    unsigned char encrypted_data[];
} sealed_data_t;


/**
 * Get random string
 * @param length size of the random string
 * @return
 */
char * random_string(size_t length) {
//     fmt::format("0xssss");
//    intx::uint256 a=1000;
//    nlohmann::json j;
    // add a number that is stored as double (note the implicit conversion of j to an object)
//    j["pi"] = 3.141;
//
//// add a Boolean that is stored as bool
//    j["happy"] = true;
//
//// add a string that is stored as std::string
//    j["name"] = "Niels";
//
//// add another null object by passing nullptr
//    j["nothing"] = nullptr;
//
//// add an object inside the object
//    j["answer"]["everything"] = 42;
//
//// add an array that is stored as std::vector (using an initializer list)
//    j["list"] = { 1, 0, 2 };
//
//// add another object (using an initializer list of pairs)
//    j["object"] = { {"currency", "USD"}, {"value", 42.99} };
    std::string str;
    return "";
}

/**
 * Generate an contract instance id
 */
int ecall_lntee_generate_contract_instance_id() { return 0; }

void join_new_participant(const char *contract_addr) {}

/**
 * Unseal data in enclave
 * @param sealed_data
 * @param sealed_data_size
 * @param data unsealed data
 * @param data_size
 * @return int result
 */
int ecall_lntee_unseal_data(
        sealed_data_t *sealed_data,
        size_t sealed_data_size,
        unsigned char **data,
        size_t *data_size) {
    return 0;
}

/**
 * Initialize the TEE account, if already generated,
 * will load from the enclave, otherwise create a new one
 * @param seed seed to create a new TEE account not necessary
 * @param pubkey TEE account pubkey
 * @param addr TEE account Addr
 * @return int
 */
int ecall_lntee_init_tee(const char *seed,
                         char *pubkey,
                         size_t pubkey_size,
                         char *addr,
                         size_t addr_size) {
    return 0;
}


/**
 * Send credit directly to counterpart
 * @param pubkey
 * @param amount
 * @param tx_str
 * @param tx_size
 */
void ecall_lntee_direct_send(const char *pubkey,
                             int amt,
                             char *tx) {

}

int ecall_lntee_direct_recv(const char *tx) {

    return -1;
}

/**
 * Process transactions from the host, should return the signed transaction
 * @param function_call
 * @param tx_str return signed tx
 */
void ecall_lntee_send(const char *function_call, char *tx_str, size_t tx_size) {


}

/**
 * Receive transaction from counter part
 * @param transaction
 * @return
 */
int ecall_lntee_recv_transaction(const char *transaction) {
    return 0;
}

/**
 * Load contract into Contract engine
 * @param contract_definition Contract detail (script and function hash)
 * @param address
 * @param addr_size
 */
void ecall_lntee_main(const char *contract_definition, char *address, size_t addr_size) {

}

void ecall_dummy(const char *msg, char *buf, size_t addr_size){ int a = 0; /* Doing nothing */ };

OE_SET_ENCLAVE_SGX(
1,    /* ProductID */
1,    /* SecurityVersion */
true, /* Debug */
1024,  /* NumHeapPages */
1024,  /* NumStackPages */
2);   /* NumTCS */

#define TA_UUID                                            \
    { /* b843807a-e05c-423c-bcfb-1062cadb483f */           \
        0xb843807a, 0xe05c, 0x423c,                        \
        {                                                  \
            0xbc, 0xfb, 0x10, 0x62, 0xca, 0xdb, 0x48, 0x3f \
        }                                                  \
    }

OE_SET_ENCLAVE_OPTEE(
TA_UUID,
100 * 1024,
6 * 1024,
0,
"1.0.0",
"lntee test")