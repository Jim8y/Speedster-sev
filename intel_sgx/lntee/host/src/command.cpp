#include "command.h"
#include <string>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <deque>
#include <iostream>
#include <eEVM/common.h>
//extern oe_enclave_t *enclave;
#include "../include/command.h"
//#include <unistd.h>
#include <eEVM/debug.h>
long int lntee::Command::ms = 0;

void lntee::Command::time_log(const char *tag) {
    struct timeval tp;
    gettimeofday(&tp, NULL);
    long int ms_2 = tp.tv_sec * 1000 * 1000 + tp.tv_usec;
    std::cout << "TIME LOG: " << tag << " >>> " << ms_2 - ms << std::endl;
    ms = ms_2;
}

void lntee::Command::time_curr(const char *tag) {
    struct timeval tp;
    gettimeofday(&tp, NULL);
    long int ms_2 = tp.tv_sec * 1000 * 1000 + tp.tv_usec;
    std::cout << "LOG: " << tag << " >>> " << ms_2 << std::endl;
}

void lntee::Command::connect(std::string peer_info) {
    INFO();
    std::map <std::string, std::string> m_props;
    using namespace boost::algorithm;
    std::vector <std::string> tokens;
    std::cout << "<port>@<ip>@<pubkey>" << std::endl;
    split(tokens, peer_info, is_any_of("@"));

    boost::asio::ip::address_v4 ipaddr =
            boost::asio::ip::address_v4::from_string(tokens[1]);

    boost::asio::ip::tcp::endpoint ep(ipaddr, std::stoi(tokens[0]));

    m_props["pubkey"] = tokens[2]; // Personal pubkey
    router->connect_to_remote(ep, m_props);
}

/**
 * Load the enclave from file
 * @param enc file path of enclave
 * @return
 */
int lntee::Command::load_enclave(const char* con_path, const char *enc) {
    oe_result_t result;
    // Create the enclave
    result = oe_create_lntee_enclave(
            enc,
            OE_ENCLAVE_TYPE_AUTO,
            OE_ENCLAVE_FLAG_DEBUG,
            NULL,
            0,
            &Global::enclave);

    if (result != OE_OK) {
        fprintf(
                stderr,
                "oe_create_lntee_enclave(): result=%u (%s)\n",
                result,
                oe_result_str(result));
        return 0;
    }

    // Generate the TEE account
    int ret = 0;
    std::string rand = random_string(100);
    ecall_lntee_init_tee(Global::enclave, &ret, rand.c_str(), Global::pubkey, Global::addr);
    this->load_contract(con_path, _CONTRACT_);
    return ret;
}

void lntee::Command::terminate_enclave() {
    if (Global::enclave)
        oe_terminate_enclave(Global::enclave);
}

/**
 * Load the contract
 */
void lntee::Command::load_contract(std::string con_path, std::string contract_name) {

    this->time_log("Satrt to Load contract");
    //    std::cout << "contract = " << contract_name << std::endl;
    //    char cwd[PATH_MAX];
    //    if (getcwd(cwd, sizeof(cwd)) != NULL) {
    //        printf("Current working dir: %s\n", cwd);
    //    }
    std::string path = con_path+"/"+ contract_name + ".json";
    DEBUG("");
    std::cout << path << std::endl;
    // Open the contract definition file
    std::ifstream contract_fstream(path);
    DEBUG("");
    if (!contract_fstream) {
        throw std::runtime_error(
                std::string("Unable to open contract definition file ") + contract_name);
    }
    DEBUG("");
    std::string str(
            (std::istreambuf_iterator<char>(contract_fstream)),
            std::istreambuf_iterator<char>());
    DEBUG("");
    // Address of the contract
    eevm::Contract_Address contractAddress;
    DEBUG("");
    ecall_lntee_main(Global::enclave, str.c_str(), (char *) &contractAddress, sizeof(contractAddress));
    this->time_log("Contract load success");
}

void lntee::Command::load_initial_state() {
}

void lntee::Command::verify_initial_state(
        oe_enclave_t *enclave, const char *msg) {
}

/**
 * This is the final step to generate a group share key
 * Generate a share key to process message exchanging
 * This shared key should be attached with the instance id
 * 1. Generate an instance id
 * 2. Generate an share key
 */
void lntee::Command::generate_share_key() {
    int ret = -1;
    // Generate an instance id
    ecall_lntee_generate_contract_instance_id(Global::enclave, &ret);
    // The share key should
}

/**
 * Direct send credit to the target without through contract
 */
void lntee::Command::direct_send(std::string target, int amt) {
    DEBUG("DIRECT SEND");
    unsigned char tx[16 + 64 + 32 + 32];
    unsigned char pubkey[64];
    Global::from_hex(target.c_str(), (char *) pubkey);
    time_log("Start the direct send");
    INFO();
//    for (int i = 0; i < 10000; i++) {
        ecall_lntee_direct_send(Global::enclave, (const char *) pubkey, amt, (char *) tx);
//    }
    time_log("End the direct send");
    INFO();
    int res =0;
    ecall_lntee_generate_contract_instance_id(Global::enclave,&res);
//    time_log("Start the direct send END");
//    for (int i = 0; i < 1000; i++) {
//    DEBUG("DIRECT SEND MSG");
//    this->time_curr("DIRECT SEND INTERNET");
    INFO();
    router->send(target, message_ptr(
            new GeneralMessage(payload::MESSAGE_TYPE::Internet_test, Global::to_hex(tx, 16 + 64 + 32 + 32))));
    DEBUG("DIRECT SEND MSG");
//    }
}

/**
 * Receive direct transaction
 * @param tx direct transaction script in hex format
 */
void lntee::Command::direct_recv(const char *tx_hex) {
    int ret = 0;
    char tx[16 + 64 + 32 + 32];
    Global::from_hex(tx_hex, (char *) tx);
    //    for (int i = 0; i < 10; ++i) {
    // time_log("Start the direct RECV");
    ecall_lntee_direct_recv(Global::enclave, &ret, tx);
    // time_log("Start the direct RECV END");
    if (ret == 0) {
        //            time_curr("End of the direct send process");
        std::cout << "SUCCESS" << std::endl;
    }
    //    }

    //    if (ret == 0) {
    //        time_curr("End of the direct send process");
    //        DEBUG("SUCCESS");
    //    }
}

/**
 * Call back function of the command class to handle
 * messages come from the peer
 * @param tx
 */
std::string lntee::Command::peer_in_come(nlohmann::json j) {
    nlohmann::json msg;
    return msg.dump();
}

/**
 * Send contract transaction
 * @param tx transactions in hex format
 */
void lntee::Command::send_contract_tx(std::string target, const char *tx, int len) {
    DEBUG("");
    len = (len % 16 == 0) ? len : (len + 1);
    char sign_tx[512]; // = new char[len];
    //    for (int i = 0; i < 2000; i++) {
    time_log("Start the contract  send");
    ecall_lntee_send(Global::enclave, tx, (char *) sign_tx);
//    time_log("Start the contract  send END");
    //    }
    int res =0;
    ecall_lntee_generate_contract_instance_id(Global::enclave,&res);

    router->send(target, message_ptr(
            new GeneralMessage(payload::MESSAGE_TYPE::Internet_test,
                               Global::to_hex((const unsigned char *) sign_tx, len))));
}

/**
 * Receive contract transaction
 * @param tx transactions in hex format
 */
void lntee::Command::recv_contract_tx(const char *tx) {

    //    for (int i = 0; i < 2000; i++) {
    time_log("Start the contract RECV");
    int ret = 0;
    ecall_lntee_recv_transaction(Global::enclave, &ret, tx);
    time_log("Start the contract  RECV END");
    //        if(ret == 0)
    //    }
    DEBUG("DIRECT SEND MSG");
}

/**
 * Get random string
 * @param length size of the random string
 * @return
 */
std::string lntee::Command::random_string(size_t length) {
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