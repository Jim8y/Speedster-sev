//
// Created by liaoj on 1/29/2020.
//

#include <host/sev_handler.h>


/**
 * Receive msg from SEV
 * Broadcast this information to the whole group
 */
inline void SEV_Handler::sev_call_back(const unsigned char *tx, size_t len, void *ptr_class) {
    //Debug();
//    cout << "MESSAGE " << tx << std::endl;

//    thread t([tx, ptr_class]() {
    //Debug();
    if (strlen((char *) tx) < 2)
        return;

    //Debug();
    SEV_Handler *p = static_cast<SEV_Handler *>(ptr_class);

    p->process_from_tee_message(tx);
//    });
//    t.join();
}


/**
 * Parse the sev account
 * @param enclave_ip  sev ip address
 * @param ptr socket callback function
 */
int SEV_Handler::connect_sev_enclave(const char *msg) {
    DEBUG("");
    try {
        nlohmann::json j = nlohmann::json::parse(msg);
        DEBUG(j["address"].get<std::string>().c_str());

        memcpy((void *) Global::pubkey_hex,
               j["pubkey"].get<std::string>().c_str(),
               PUBKEY_LEN * 2 + 2);
        memcpy((void *) Global::addr_hex, j["address"].get<std::string>().c_str(),
               ADDRESS_LEN * 2 + 2);
        std::cout << "Address: " << j["address"].get<std::string>() << std::endl;
        std::cout << "Connect: "
                  << Global::port << "@"
                  << Global::ip << "@"
                  << Global::pubkey_hex << std::endl;

//        DEBUG("");
//
//        /** connect to the sev enclave */
//        sev_enclave_socket._init(sev_call_back, (void *) this, PORT_SEV, enclave_ip);
//        sev_enclave_socket._connect();

        return 0;
    }
    catch (exception &e) {
        cout << e.what() << endl;
        return -1;
    }
}

/**
 * Direct send credit to the target without through contract
 */
void SEV_Handler::host_cmd_direct_send(std::string target, int amt) {
    DEBUG("DIRECT SEND");
    unsigned char tx[16 + 64 + 32 + 32];
//    unsigned char pubkey[64];
//    lntee::from_hex(target.c_str(), (char *) pubkey);
//    time_log("Start the direct send");
//    for (int i = 0; i < 10000; i++) {

//        ecall_lntee_direct_send(Global::enclave, (const char *) pubkey, 1, (char *) tx);

//        time_log("Direct transaction send to END");
//    time_log("Start the direct send END");

//    time_log("Start the direct send");
//    for (int i = 0; i < 10000; i++)
    nlohmann::json j;
    j["cmd"] = payload::MESSAGE_TYPE::HOST_CMD_DIRECT_SEND_TX;
    j["pubkey"] = target;
    j["amt"] = amt;
    send_msg_through_sock(j);
//        router->send(target, message_ptr(
//                new GeneralMessage(payload::MESSAGE_TYPE::Direct_tranaction, lntee::to_hex(tx, 16 + 64 + 32 + 32))));
//        time_log("End a tranaction-------------------------------------------------");
//    }
//    }
//    time_log("Start the direct send END");
}

/**
 * Receive direct transaction
 * @param tx direct transaction script in hex format
 */
void SEV_Handler::host_cmd_direct_recv(const char *tx_hex) {
    int ret = 0;
    char tx[16 + 64 + 32 + 32];
    lntee::from_hex(tx_hex, (char *) tx);
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

void SEV_Handler::process_from_tee_message(const unsigned char *msg) {

    nlohmann::json j = nlohmann::json::parse(msg);

//    DEBUG(j["address"].get<std::string>().c_str());
    payload::MESSAGE_TYPE cmd = static_cast<payload::MESSAGE_TYPE>(j["cmd"].get<int>());
    switch (cmd) {
        case payload::MESSAGE_TYPE::SEV_CMD_ACCOUNT_RET:

            break;

        case payload::MESSAGE_TYPE::SEV_CMD_DIRECT_SEND_TX_RES: {
            DEBUG(msg);

//            _cmd->direct_send(j);
            this->call_back(msg, this->cmd_ptr);

            break;
        }
        case payload::MESSAGE_TYPE::SEV_CMD_INVOKE_SEND_TX_RES: {
            this->call_back(msg, this->cmd_ptr);
            break;
        }
        default:
            break;
    }
//    int cmd = (int) msg[0];
//    //Debug();
////    cout << "- SEND : " << lntee::to_hex(msg, 512) << endl;
//    switch (cmd) {
//        case HOST_REQUEST_SEV_ACCOUNT:
////            cout << "SEV account : " << lntee::to_hex((unsigned char *) &msg[2], ADDRESS_LEN) << endl;
////            memcpy((unsigned char *) &bin_sev_address, &msg[2], ADDRESS_LEN);
////            contractSync.load_b_sev_address((unsigned char *) &bin_sev_address);
//            break;
//
//        default:
//            //Debug();
////            cout << "- SEND : " << lntee::to_hex(msg, 512) << endl;
//            broad_cast(msg);
//            break;
//    }
}

/**
 * Send contract transaction
 * @param tx transactions in hex format
 */
void SEV_Handler::host_cmd_invoke_send_tx(const char *tx) {

    nlohmann::json j;
    j["cmd"] = payload::MESSAGE_TYPE::HOST_CMD_INVOKE_SEND_TX;
    j["tx"] = tx;
//    std::cout <<"<<<<<<<<<<<< " << j.dump() << std::endl;
    send_msg_through_sock(j);
}


void SEV_Handler::process_to_tee_message(const unsigned char *msg) {
//    int cmd = (int) msg[0];
//    //Debug();
////    cout << "- SEND : " << lntee::to_hex(msg, 512) << endl;
//    switch (cmd) {
//        case HOST_REQUEST_SEV_ACCOUNT:
////            cout << "SEV account : " << lntee::to_hex((unsigned char *) &msg[2], ADDRESS_LEN) << endl;
////            memcpy((unsigned char *) &bin_sev_address, &msg[2], ADDRESS_LEN);
////            contractSync.load_b_sev_address((unsigned char *) &bin_sev_address);
//            break;
//
//        default:
//            //Debug();
////            cout << "- SEND : " << lntee::to_hex(msg, 512) << endl;
//            broad_cast(msg);
//            break;
//    }
}

void SEV_Handler::send_msg_through_sock(nlohmann::json &j) {
    DEBUG("");
    std::string str = j.dump();
    DEBUG("")
    DEBUG(str.c_str());
}