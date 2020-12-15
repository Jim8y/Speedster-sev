#include <host/command.h>
#include <string>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <deque>
#include <iostream>
#include <eEVM/common.h>

long int lntee::Command::ms = 0;
int lntee::Command::count = 10000;

void lntee::Command::cmd_call_back(const unsigned char *tx, void *cp) {
    Command *c = static_cast<Command *>(cp);

    string tg = "0xc2f64c9051839d9b6f692e24face387a6a99e1c31e63fad39cef32d3b3e08657969877bc78c05b63c22ee8f04f5fd10ba7dd16a12d91561a8b920c0776e577c2";

    nlohmann::json j = nlohmann::json::parse(tx);
    payload::MESSAGE_TYPE cmd = static_cast<payload::MESSAGE_TYPE>(j["cmd"].get<int>());

    switch (cmd) {
        case payload::MESSAGE_TYPE::SEV_CMD_DIRECT_SEND_TX_RES: {

            c->router->send(tg, message_ptr(
                    new GeneralMessage(payload::MESSAGE_TYPE::HOST_CMD_DIRECT_SEND_TX, j.dump())));

            if (lntee::Command::count > 0) {
                DEBUG("");
                c->direct_send(tg, 1);
                lntee::Command::count--;
            } else {
                c->time_curr("Start the direct send END");
            }
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
    std::map<std::string, std::string> m_props;
    using namespace boost::algorithm;
    std::vector<std::string> tokens;
    std::cout << "<port>@<ip>@<pubkey>" << std::endl;
    split(tokens, peer_info, is_any_of("@"));

    boost::asio::ip::address_v4 ipaddr =
            boost::asio::ip::address_v4::from_string(tokens[1]);

    boost::asio::ip::tcp::endpoint ep(ipaddr, std::stoi(tokens[0]));

    m_props["pubkey"] = tokens[2]; // Personal pubkey
    DEBUG("");
//    std::cout <<m_props<<std::endl;

    router->connect_to_remote(ep, m_props);
}

/**
 * Load the enclave from file
 * @param enc/ip file path of enclave
 * @return
 */
int lntee::Command::load_enclave(const char *enc) {

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

void lntee::Command::terminate_enclave() {
//    if (Global::enclave)
//        oe_terminate_enclave(Global::enclave);
}

/**
 * Load the contract
 */
void lntee::Command::load_contract(std::string contract_name) {


//    this->time_log("Satrt to Load contract");
    std::string path = "/home/sev/CLionProjects/LNTEECMAKE/lntee/contract/json/" + contract_name + ".json";
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
//    DEBUG("");
//   std::string str = "{\"bin\":\"608060816050b0032\",\"hashes\":{\"allowance(address,address)\":\"dd62ed3e\",\"approve(address,uint256)\":\"095ea7b3\",\"balanceOf(address)\":\"70a08231\",\"totalSupply()\":\"18160ddd\",\"transfer(address,uint256)\":\"a9059cbb\",\"transferFrom(address,address,uint256)\":\"23b872dd\"}}";

//   std::cout <<"Length of String = "<<str.length() <<std::endl;
// Address of the contract


    DEBUG("");
    nlohmann::json j;
//    j["cmd"] = payload::MESSAGE_TYPE::HOST_CMD_LOAD_CONTRACT;
    j["contract"] = str;

    router->send(SEV_SOCK_TOKEN, message_ptr(
            new GeneralMessage(payload::MESSAGE_TYPE::HOST_CMD_LOAD_CONTRACT, j.dump())));
//    DEBUG(j.dump().c_str());
//    nlohmann::json jj = nlohmann::json::parse(str);
//    send_msg_through_sock(j);
//    ecall_lntee_main(Global::enclave, str.c_str(), (char *) &contractAddress, sizeof(contractAddress));
//    this->time_log("Contract load success");
}

void lntee::Command::load_initial_state() {
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
void lntee::Command::generate_share_key() {
    int ret = -1;
    // Generate an instance id
//    ecall_lntee_generate_contract_instance_id(Global::enclave, &ret);
    // The share key should
}

/**
 * Direct send credit to the target without through contract
 */
void lntee::Command::direct_send(std::string target, int amt) {
//    DEBUG("DIRECT SEND");

    nlohmann::json j;
    j["pubkey"] = target;
    j["amt"] = amt;
    DEBUG(j.dump().c_str());
    string str = j.dump();

    router->send(SEV_SOCK_TOKEN, message_ptr(
            new GeneralMessage(payload::MESSAGE_TYPE::HOST_CMD_DIRECT_SEND_TX, str)));
}

/**
 * Receive direct transaction
 * @param tx direct transaction script in hex format
 */
void lntee::Command::direct_recv(const char *tx_hex) {
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
    router->send(SEV_SOCK_TOKEN, message_ptr(
            new GeneralMessage(payload::MESSAGE_TYPE::HOST_CMD_DIRECT_RECV_TX, tx_hex)));

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
//    len = (len % 16 == 0) ? len : (len + 1);
//    char sign_tx[512]; // = new char[len];
    nlohmann::json j;
    j["tx"] = tx;

    string str = j.dump();
    DEBUG(str.c_str());
    router->send(SEV_SOCK_TOKEN, message_ptr(
            new GeneralMessage(payload::MESSAGE_TYPE::HOST_CMD_INVOKE_SEND_TX, str)));
}

/**
 * Receive contract transaction
 * @param tx transactions in hex format
 */
void lntee::Command::recv_contract_tx(const char *tx) {

    //    for (int i = 0; i < 2000; i++) {
//    time_log("Start the contract RECV");
    int ret = 0;
    std::cout << "RECV " << tx << std::endl;
//    ecall_lntee_recv_transaction(Global::enclave, &ret, tx);
//    time_log("Start the contract  RECV END");
    //        if(ret == 0)
    //    }
    DEBUG("DIRECT SEND MSG");
//    HOST_CMD_INVOKE_SEND_TX
    string str = std::string(tx);
    std::cout << "RECV " << str << std::endl;
    std::cout << "RECV " << str.length() << std::endl;
    router->send(SEV_SOCK_TOKEN, message_ptr(
            new GeneralMessage(payload::MESSAGE_TYPE::HOST_CMD_INVOKE_RECV_TX, str)));
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