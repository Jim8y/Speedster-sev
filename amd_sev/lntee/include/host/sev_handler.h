//
// Created by liaoj on 1/29/2020.
//
#include <iostream>
#include <fstream>
#include <string>
#include <eEVM/simple/simpleglobalstate.h>
#include <eEVM/debug.h>
#include <socket/config.h>
#include <thread>
#include <common/tee_handler.h>
#include <common/message.h>
#include <nlohmann/json.hpp>
#include <host/Global.h>

#ifndef EVM_SEV_HANDLER_H
#define EVM_SEV_HANDLER_H


using namespace std;


class SEV_Handler : public tee_handler {

public:
    cmd_call_back_ptr call_back;
    void *cmd_ptr;

public:


    void set_cmd_call_back(cmd_call_back_ptr callback, void *cmd_ptr) {
        call_back = callback;
        this->cmd_ptr = cmd_ptr;
    }

    void send_to_sev(nlohmann::json &j);

    inline static void sev_call_back(const unsigned char *tx, size_t len, void *ptr_class);

    /**
     * parse the sev account
     * @param enclave_ip  sev ip address
     * @param ptr socket callback function
     */
    int connect_sev_enclave(const char *msg);


    /**
      * Directly send credit to target
      * @param target  target public key in hex format
      * @param amt amount to send
      */
    void host_cmd_direct_send(std::string target, int amt);

    /**
     * Receive direct transaction
     * @param tx direct transaction script
     */
    void host_cmd_direct_recv(const char *tx_hex);

    /**
     * Send command to invoke contract
     * @param target
     * @param tx
     * @param len
     */
    void host_cmd_invoke_send_tx(const char *tx);

    void process_to_tee_message(const unsigned char *msg);

    void process_from_tee_message(const unsigned char *msg);

    void send_msg_through_sock(nlohmann::json &j);
};


#endif //EVM_SEV_HANDLER_H
