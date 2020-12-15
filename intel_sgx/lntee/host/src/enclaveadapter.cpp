#include "lntee_u.h"
#include "command.h"
#include <openenclave/host.h>
#include "Global.h"

/**
 * call for a log from the enclave
 * @param tag tag of the log
 */
void ocall_lntee_time_log()
{
    lntee::Command::time_log("Log from the enclave");
}

/**
 * Ocall function: send transaction to target
 *
 * @param target_pubkey send target
 * @param transaction tx content
 *
 */
void ocall_lntee_send_transaction(const char *target_pubkey, const char *transaction)
{
    //    client.
}

/**
 * Broadcast a transaction to participants of an contract instance
 * @param tx transaction in hex version or maybe in dumped json version
 * @param instance_id limit the broadcast range
 */
void ocall_lntee_broadcast(const char *tx, const char *instance_id)
{
}

/**
 * Ocall function: Get the pubkey from the enclave
 * @param pubkey pubkey hex versoin
 * @param len size of the hex pubkey
 */
void ocall_lntee_pubkey(const char *pubkey, size_t len)
{
}

//void test_resv_call_back(const char *tx, size_t len) {
//    // std::cout << "Call Back!" << std::endl;
//    // cout << tx << endl;
////    int ret = 0;
////    char *tx_temp = const_cast<char *>(tx);
////    tx_temp[0] = RECV_TRANSACTION;
////    ecall_lntee_recv_transaction(enclave, &ret, tx_temp);
//}
//
//void test_send_contract_tx(oe_enclave_t *enclave) {
////    int amount = 0;
////    std::string addr = "";
////    while (true) {
////        std::cout << "Input the target address and amount of coin to send:"
////                  << std::endl;
////
////        std::cin >> addr >> amount;
////        char tx[512] = {'0'};
////
////        size_t tx_size = 512;
////
//////        eevm::Address target;
//////        _from_hex(addr.c_str(), (char *) &target);
////        time_t seconds = time(NULL);
////        cout << "Current time = " << (long long) seconds << endl;
////
////        char *addr_c = (char *) addr.c_str();
////        for (int i = 0; i < 2000; ++i) {
////            ecall_lntee_send(enclave, (const char *) addr_c, amount, tx, tx_size);
//////             std::cout << "TX from enclave: " << tx << std::endl;
////            sck->_send(tx, tx_size);
////        }
////    }
//}
