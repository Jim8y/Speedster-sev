#include "lntee_t.h"
#include "dispatcher.h"
#include "eEVM/bigint.h"
#include "eEVM/common.h"
#include "eEVM/simple/simpleglobalstate.h"
#include "eEVM/transaction.h"
#include "mbedtls/sha256.h"
#include "mbedtls/platform.h"
#include "message.h"
#include "sgx_contract.h"
#include "shared.h"
#include "wallet.h"
#include <random>
#include <stdio.h>
#include "eth_aes.h"
#include "eth_ecdsa.h"
#include "eth_ecp.h"
#include <map>
#include <eEVM/debug.h>

//#define LEN_128
//#define LEN_256
//#define LEN_1024
#define _TEST_ECDSA_
//#define _TEST_AES_

static ecall_dispatcher dispatcher;
static Contract_Instance contract_instance;
static SGX_Contract contract;

/**
 * Get random string
 * @param length size of the random string
 * @return
 */
std::string random_string(size_t length) {
    LOG("");
    srand(time(NULL));
    auto randchar = []() -> char {
        const char charset[] =
                "0123456789"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "abcdefghijklmnopqrstuvwxyz";
        const size_t max_index = (sizeof(charset) - 1);
        return charset[rand() % max_index];
    };
    std::string str(length, 0);
    std::generate_n(str.begin(), length, randchar);
    return str;
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
    LOG("");
    printf("Enclave: unseal_data\n");
    return dispatcher.unseal_data(sealed_data, sealed_data_size, data, data_size);
}

/**
 * Initialize the TEE account, if already generated,
 * will load from the enclave, otherwise create a new one
 * @param seed seed to create a new TEE account not necessary
 * @param pubkey TEE account pubkey
 * @param addr TEE account Addr
 * @return int
 */
int ecall_lntee_init_tee(const char *seed, const char *pubkey, const char *addr) {
    LOG("");
    unsigned char opt_mgs[128] = "hello world";
    size_t opt_msg_len = strlen((char *) opt_mgs);
    unsigned char *data = (unsigned char *) seed;
    size_t data_size = strlen((char *) data) + 1;
    sealed_data_t *sealed_data = NULL;

    initial_aes();

    size_t sealed_data_size = 0;
    int result = dispatcher.seal_data(
            POLICY_UNIQUE,
            opt_mgs,
            opt_msg_len,
            data,
            data_size,
            &sealed_data,
            &sealed_data_size);

    if (result != OE_OK) {
        cout << "Host: seal_data failed with " << result << endl;
    }

    // Generate prikey according to the hash of sealing result
    mbedtls_sha256_ret(
            (unsigned char *) sealed_data, sealed_data_size,
            (unsigned char *) &Wallet::Instance()->get_account()->prikey, 0);

    provision_ecdsa_key((char *) &Wallet::Instance()->get_account()->prikey, SECKEY_LEN);
    get_address((unsigned char *) &Wallet::Instance()->get_account()->pubkey,
                (unsigned char *) &Wallet::Instance()->get_account()->address);

    std::cout << "\n------------------------\n"
              << std::endl;


    std::cout << "ADDRESS: "
              << eevm::to_hex_string(Wallet::Instance()->get_account()->address) << std::endl;
    std::cout << "PRIKEY : "
              << eevm::to_hex_string(Wallet::Instance()->get_account()->prikey)
              << std::endl;
    std::cout << "PUBKEY :"
              << eevm::to_hex_string(Wallet::Instance()->get_account()->pubkey)
              << std::endl;

    std::cout << "\n------------------------\n"
              << std::endl;
    // For test
    Wallet::Instance()->set_balance(1000000);

    // Copy TEE pubkey to the host
    memcpy((void *) pubkey,
           eevm::to_hex_string(Wallet::Instance()->get_account()->pubkey).c_str(),
           PUBKEY_LEN * 2+2);
    memcpy((void *) addr,
           eevm::to_hex_string(Wallet::Instance()->get_account()->address).c_str(),
           ADDRESS_LEN * 2+2);

    initial_aes();


#ifdef LEN_128
#define LEN 128
    char msg[LEN] = "nuocLTIIeYWhZjCg05taqAjPMw1usrHVN3zfXNK4s3LhJIg6mQzTFHTTfC0vnwC6uMb4VCdZyTBBA2bJRnzDlpRoKciVDNBoH44QO5YkudHq66zYh5HozOuGWdGK7M1";
#endif

#ifdef LEN_256
#define LEN 256
    char msg[LEN] ="1NqnGodGaHWEfdLZUqrCkT9mw4QsPZgOWSErhNxJwuAhhAHwNSDNhiv2pzy8N6YcDH6a1Owhn3UTz2UBJM1iw7aXNgkzm28ivLmCST9rqUyZYFuZ5sm6x5GduXburpbLK1NhJxe5Zh49XcxkKi2ZdVKG88coZSebkRUFJdbI7jrczHvc4vPLINzDUHwq1MmoWQpxqM7kspHXUDXop9AGa0iCyOg0jvgy4fefGqS97H21aZSRgYFqZXOtM4PgpCw";
#endif

#ifdef LEN_1024
#define LEN 1024
    char msg[LEN] = "ktsER8hRaw1hr3QkjO8QXd7G2ldhpTe7FcTTzt6WE0beFDTZJJ5H5z96HWSlgXdDLhEg4iyUz9VVlBky330XxvRLwkABgWIj8MCL2UtAMdiMOsQR2WfzRQoFo9W3Vx3Ci4JM2SnnyYzVCHoYs4svweKXIuOAun0mg321W02uxerjst0FEKyFbuZDnJHOpwkblHGXKJ9Z9ZR5ghwP3grlKFGXO6hH3W2RNJXOhx8BD6DocUEcTk8SIe4ElQHvPA53KyEYZgGTpF5NWR4G8IVZNekl7D8DKiyfNROJ0eViWQR7hw1krF0xEF9Jo8V0mc3eVgvEtwYbjoEhIZxIVO2TQTBrvvNAZHgQaJk2gnw4c7cnA6cobDqzZ3jX6W6cKAlh74xgmscjx75JkjIoMuU9FJy9EBlmkcuSQz63v9bvFLkzwsl5vpBhVpfpkuv06sf31my79kzymiMYGbIUdIKO3STdTmSaqbIoXRBHON3qMe3Cjw67wd2mVJv8dzTdg3t5R9CmIw8JIhnfyQfcUFIPhKIO6c8A57GzDlhfBcJZzoW4sn2EGab3YEB47wesiIv69DBVWahMWWQAMYWDEbKgAlbFh6aIlKc6zQtPTIOrjqZv99Gchx3O7A3jKndB9E9IgOB52kuQgG6EGrPiZDqgagCLnkKz96XGUAy2BNzDZL0M14nxOCkXlQjvM5phALG2c3BfqOsUhuJ6wmruY3GgX8plqDbVIL4hbWCRe62FFZrY8X3sQmqfB6K0ktxlcyLsGnVzuWWgHAZGPVCml0tEAFCgABrUqevohDC2YqXIrI7NTdL0MnYzj2uMytCSbhwgunLhcnKjQ5py6LuBmNvyU16HXke7pmPlaAra8xKYNOwZkH8jwHVLS55A0jiuZ9tz8IVQUq9jUvmFZ0N0C3lIVwUmnupAJGcEiI0B62LUMXs6qV2zLKPWqVUl0OwZ7PPKQAogfNbPXEo8A6aL9Q54R9PgKZq5qb7G3X6QE7kEMVzytAKoMPjDCvCSrEfvj1D";
#endif

#ifdef _TEST_ECDSA_
//    unsigned char output1[32];
//
//    mbedtls_sha256_ret((const unsigned char *) msg, LEN, output1, 0);
////    for (int i = 0; i < 10000; i++) {
//    ocall_lntee_time_log();
//        ecdsa_sign(output1);
////    }
//    ocall_lntee_time_log();
#endif

#ifdef _TEST_AES_

    char cipher[LEN] = {'\0'};
    char plain[LEN] = {'\0'};
////    print_hex("Method 1", output1, sizeof output1);
    aes_encrypt((unsigned char *) msg, LEN / 16, (unsigned char *) cipher);
    ocall_lntee_time_log();
    for (int i = 0; i < 10000; i++) {
        aes_decrypt((char *) cipher, LEN / 16, (unsigned char *) msg);
    }

    ocall_lntee_time_log();
        char cipher[1024] = {'\0'};
    char plain[1024] = {'\0'};
    DEBUG("");
    ocall_lntee_time_log();
    for (int i = 0; i < 10000; i++) {
        aes_encrypt((unsigned char *) msg, 64, (unsigned char *) cipher);
        aes_decrypt((char *) cipher, 64, (unsigned char *) plain);
    }
    ocall_lntee_time_log();
    DEBUG("");
    aes_finish();
    aes_test();
#endif
    return -1;
}

/**
 * Calls by the host to handle command from user
 * Process transactions in Contract engine
 * @param tx transaction script
 */
void process_transaction(eevm::Transaction *tx) {
    LOG("");
    eevm::Address target;
    memcpy((void *) &target, (char *) &tx->target, ADDRESS_LEN);
    const auto success = contract.transfer(tx->origin, target, tx->value);
    if (!success) {
        throw std::runtime_error("Expected transfer to succeed, but it failed");
    }
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
    char cipher[80];
    char plain[80];
    unsigned char py[64];
//    Wallet::Instance()->direct_send(*(eevm::Pubkey *) pubkey, amt);
for (int i = 0; i < 10000; i++)
{
    if (Wallet::Instance()->direct_send(*(eevm::Pubkey *) pubkey, amt)) {
        // 0 -15 bytes amt
        // 16 - 79 bytes local_pubkey
//        char cipher[80];
//        char plain[80];
//        DEBUG("");
//        memcpy((void *) tx, (char *) &amt, sizeof(int));
//        memcpy((void *) (tx + 16), (char *) &Wallet::Instance()->get_account()->pubkey, PUBKEY_LEN);
//        DEBUG("");
//        aes_encrypt((unsigned char *) tx, 5, (unsigned char *) cipher);
//        aes_decrypt((char *) cipher, 5, (unsigned char *) plain);
//        char msg[1024] = "helloworldhelloworldhelloworldhelloworldhelloworldhelloworldhelloworldhelloworldhelloworldhelloworldhelloworldhelloworld";
#ifdef _TEST_ECDSA_

//        //        char msg[1024] = "helloworldhelloworldhelloworldhelloworldhelloworldhelloworldhelloworldhelloworldhelloworldhelloworldhelloworldhelloworld";
//        unsigned char output1[32];
//    char cipher[1024] = {'\0'};
//    char plain[1024] = {'\0'};
//    print_hex("Method 1", output1, sizeof output1);

//    for (int i = 0; i < 10000; i++) {
//        * \param input    buffer holding the  data
//        * \param ilen     length of the input data
//        * \param output   SHA-224/256 checksum result
//        * \param is224    0 = use SHA256, 1 = use SHA224
//        mbedtls_sha256_ret((const unsigned char *) tx, 144, output1, 0);
//        ecdsa_sign(output1);
#else
//        aes_encrypt((unsigned char *) tx, 5, (unsigned char *) cipher);
//        aes_decrypt((char *) cipher, 5, (unsigned char *) plain);
#endif
    } else {
        cout << "Balance not enough" << endl;
    }
}

}

int ecall_lntee_direct_recv(const char *tx) {
    DEBUG("");
    int amt = 0;
    char pubkey[PUBKEY_LEN];
    memcpy((void *) &amt, tx, sizeof(int));
    memcpy((void *) pubkey, tx + 16, PUBKEY_LEN);

    if (Wallet::Instance()->direct_receive(*(eevm::Pubkey *) pubkey, amt))
        return 0;

    return -1;
}

/**
 * Process transactions from the host, should return the signed transaction
 * @param function_call
 * @param tx_str return signed tx
 */
void ecall_lntee_send(const char *function_call, char *tx_str) {
    DEBUG("");

    eevm::Code script = lntee::from_hex(function_call);

    int size = (script.size() % 16 == 0) ? script.size() / 16 : (script.size() / 16 + 1);

    contract.invoke(Wallet::Instance()->get_account()->address, script);

    contract.invoke(Wallet::Instance()->get_account()->address, script);
        DEBUG("SUCCESS");

#ifdef _TEST_ECDSA_

        unsigned char output1[32];
//        this->time_log("End Invoke");
        // DEBUG("SUCCESS");



        ////    print_hex("Method 1", output1, sizeof output1);
        ////    ocall_lntee_time_log();
        //
        ////    for (int i = 0; i < 10000; i++) {
        mbedtls_sha256_ret((unsigned char *) &script[0], script.size(), output1, 0);
        ecdsa_sign(output1);

//        this->time_log("End Enc");
//    this->time_curr("Start Dec");
        //    }
        //        ocall_lntee_time_log();
        //    } else {
        //        DEBUG("FAILED");
        //        ocall_lntee_time_log();
        //    }
#else
        char *plain = new char[size * 16];
        memset(plain, 0, size * 16);

        memcpy((void *) plain, (unsigned char *) &script[0], script.size());
        DEBUG("");
        aes_encrypt((unsigned char *) plain, size, (unsigned char *) tx_str);

         aes_decrypt((char *) tx_str, size, (unsigned char *) plain);
//        char cipher[LEN] = {'\0'};
//        char plain[LEN] = {'\0'};
//        aes_encrypt((unsigned char *) msg, LEN/16, (unsigned char *) cipher);
////    this->time_log("End Enc");
////    this->time_log("Start Dec");
//        aes_decrypt((char *) cipher, LEN/16, (unsigned char *) plain);
//    this->time_log("End Dec");
#endif

//    } else {
//        DEBUG("FAILED");
//    }

}

/**
 * Receive transaction from counter part
 * @param transaction
 * @return
 */
int ecall_lntee_recv_transaction(const char *transaction) {
    LOG("");

    int len = strlen(transaction);
    unsigned char *enc_tx = new unsigned char[len];
    memset(enc_tx, 0, len);
    lntee::from_hex(transaction, (char *) enc_tx);
    eevm::Code plain;
    plain.resize(len);
    aes_decrypt((char *) enc_tx, len / 16, (unsigned char *) plain.data());
    if (contract.invoke(Wallet::Instance()->get_account()->address, plain)) {
        DEBUG("SUCCESS");
        return 0;
    } else {
        DEBUG("FAILED");
        return -1;
    }
}

/**
 * Load contract into Contract engine
 * @param contract_definition Contract detail (script and function hash)
 * @param address
 * @param addr_size
 */
void ecall_lntee_main(const char *contract_definition, char *address, size_t addr_size) {
    LOG("");
    contract.loadContract(nlohmann::json::parse(contract_definition), &Wallet::Instance()->get_account()->address);
}

void ecall_dummy(){ int a=0 ; /* Doing nothing */ };