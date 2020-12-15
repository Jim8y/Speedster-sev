#include <enclave/enclave_command.h>
#include <string>
#include <deque>
#include <iostream>
#include <eEVM/common.h>

long int lntee::enclave_command::ms = 0;
int lntee::enclave_command::count = 10000;

void lntee::enclave_command::cmd_call_back(const unsigned char *tx, void *cp) {

    enclave_command *c = static_cast<enclave_command *>(cp);
    DEBUG("");
    DEBUG(tx);
    nlohmann::json j = nlohmann::json::parse(tx);
    payload::MESSAGE_TYPE cmd = static_cast<payload::MESSAGE_TYPE>(j["cmd"].get<int>());
    switch (cmd) {
        case payload::MESSAGE_TYPE::SEV_CMD_DIRECT_SEND_TX_RES: {
            c->direct_send(j);
            break;
        }
        case payload::MESSAGE_TYPE::SEV_CMD_INVOKE_SEND_TX_RES: {
            DEBUG("");
            string target = "0x9cb7efb8881d42071935df9ded99d235570557ead9058a95324a87f1ac0e95678e2c5685e261087fec0b747516e67e9db5950a67d0b22d09c8c786404893ea49";
            string tx = j["tx"].get<string>();
            c->router->send(target, message_ptr(
                    new GeneralMessage(payload::MESSAGE_TYPE::HOST_CMD_INVOKE_SEND_TX, tx)));
            time_curr("END OF CONTRACT TRANSACTION");
            break;
        }
        default:
            break;
    }

    DEBUG("");
//    payload::MESSAGE_TYPE cmd = static_cast<payload::MESSAGE_TYPE>(j["cmd"].get<int>());
//    j["tx"] = lntee::to_hex((const unsigned char *)tx,16*5);
//    j["pubkey"] =pubkey;
}

void lntee::enclave_command::time_log(const char *tag) {
    struct timeval tp;
    gettimeofday(&tp, NULL);
    long int ms_2 = tp.tv_sec * 1000 * 1000 + tp.tv_usec;
    std::cout << "TIME LOG: " << tag << " >>> " << ms_2 - ms << std::endl;
    ms = ms_2;
}

void lntee::enclave_command::time_curr(const char *tag) {
    struct timeval tp;
    gettimeofday(&tp, NULL);
    long int ms_2 = tp.tv_sec * 1000 * 1000 + tp.tv_usec;
    std::cout << "LOG: " << tag << " >>> " << ms_2 << std::endl;
}

/**
 * Load the enclave from file
 * @param enc/ip file path of enclave
 * @return
 */
int lntee::enclave_command::load_enclave(const char *enc) {

    int ret = 0;

#ifdef ENCLAVE_SGX
    oe_result_t result;
     Create the enclave
    result = oe_create_lntee_enclave(
            enc, OE_ENCLAVE_TYPE_AUTO, OE_ENCLAVE_FLAG_DEBUG, NULL, 0, &Global::enclave);
    if (result != OE_OK) {
        fprintf(
                stderr,
                "oe_create_lntee_enclave(): result=%u (%s)\n",
                result,
                oe_result_str(result));
        return 0;
    }
    // Generate the TEE account
//    int ret = 0;
    std::string rand = random_string(100);
    ecall_lntee_init_tee(Global::enclave, &ret, rand.c_str(), Global::pubkey_hex, Global::addr_hex);
    this->load_contract("gomoku_combined");

#elseif ENCLAVE_SEV
//   int ret = SEV_Handler::connect_sev_enclave(enc);
#endif
    return ret;
}

void lntee::enclave_command::terminate_enclave() {
//    if (Global::enclave)
//        oe_terminate_enclave(Global::enclave);
}

/**
 * Load the contract
 */
void lntee::enclave_command::load_contract(std::string contract_name) {

    this->time_log("Satrt to Load contract");

    std::string path = "../contract/json/" + contract_name + ".json";
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

//    ecall_lntee_main(Global::enclave, str.c_str(), (char *) &contractAddress, sizeof(contractAddress));
    this->time_log("Contract load success");
}

void lntee::enclave_command::load_initial_state() {
}

//void lntee::Command::verify_initial_state(
//        oe_enclave_t *enclave, const char *msg) {
//}

/**
 * This is the final step to generate a group share key
 * Generate a share key to process message exchanging
 * This shared key should be attached with the instance id
 * 1. Generate an instance id
 * 2. Generate an share key
 */
void lntee::enclave_command::generate_share_key() {
    int ret = -1;
    // Generate an instance id
//    ecall_lntee_generate_contract_instance_id(Global::enclave, &ret);
    // The share key should
}

/**
 * Direct send credit to the target without through contract
 */
void lntee::enclave_command::direct_send(nlohmann::json &j) {
    DEBUG("DIRECT SEND");
//    unsigned char tx[16 + 64 + 32 + 32];
//    unsigned char pubkey[64];
//    lntee::from_hex(target.c_str(), (char *) pubkey);
//    time_log("Start the direct send");
//    for (int i = 0; i < 10000; i++) {

//        ecall_lntee_direct_send(Global::enclave, (const char *) pubkey, 1, (char *) tx);

//        time_log("Direct transaction send to END");
//    time_log("Start the direct send END");

//    time_log("Start the direct send");
//    for (int i = 0; i < 10000; i++)
    string target = j["pubkey"].get<string>();
    string tx = j["tx"].get<string>();
    router->send(target, message_ptr(
            new GeneralMessage(payload::MESSAGE_TYPE::HOST_CMD_DIRECT_SEND_TX, tx)));
//        time_log("End a tranaction-------------------------------------------------");
//    }
////    }
    time_curr("Start the direct send END");
    string tg = "0x9cb7efb8881d42071935df9ded99d235570557ead9058a95324a87f1ac0e95678e2c5685e261087fec0b747516e67e9db5950a67d0b22d09c8c786404893ea49";

//    if(count >0)
//    sev_handler.host_cmd_direct_send(tg, count--);
}

/**
 * Receive direct transaction
 * @param tx direct transaction script in hex format
 */
void lntee::enclave_command::direct_recv(const char *tx_hex) {
    DEBUG("");
    int ret = 0;
//    char tx[16 + 64 + 32 + 32];
//    lntee::from_hex(tx_hex, (char *) tx);
    //    for (int i = 0; i < 10; ++i) {
    // time_log("Start the direct RECV");
//    ecall_lntee_direct_recv(Global::enclave, &ret, tx);
    // time_log("Start the direct RECV END");
//    if (ret == 0) {
//        //            time_curr("End of the direct send process");
//        std::cout << "SUCCESS" << std::endl;
//    }
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
std::string lntee::enclave_command::peer_in_come(nlohmann::json j) {
    nlohmann::json msg;
    return msg.dump();
}

/**
 * Send contract transaction
 * @param tx transactions in hex format
 */
void lntee::enclave_command::send_contract_tx(std::string target, const char *tx, int len) {
    DEBUG("");
    len = (len % 16 == 0) ? len : (len + 1);
    char sign_tx[512]; // = new char[len];
//    time_log("Start the contract  send");
//    for (int i = 0; i < 10000; i++) {
//    ecall_lntee_send(Global::enclave, tx, (char *) sign_tx);
//    this->sev_handler.host_cmd_invoke_send_tx(tx);

//    }
//    time_log("Start the contract  send END");

}

/**
 * Receive contract transaction
 * @param tx transactions in hex format
 */
void lntee::enclave_command::recv_contract_tx(const char *tx) {

    //    for (int i = 0; i < 2000; i++) {
    time_log("Start the contract RECV");
    int ret = 0;
//    std::cout << "RECV " << tx << std::endl;
//    ecall_lntee_recv_transaction(Global::enclave, &ret, tx);
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
std::string lntee::enclave_command::random_string(size_t length) {
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