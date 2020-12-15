//
// Created by sev on 3/25/20.
//

#ifndef LNTEE_ENCLAVE_PROTOCOL_H
#define LNTEE_ENCLAVE_PROTOCOL_H

#include "libf2f/protocol.h"
#include "libf2f/message.h"
#include <common/message.h>
#include <enclave/enclave.h>
#include <nlohmann/json.hpp>

#include <eEVM/common.h>
#include <common/wallet.h>

using namespace libf2f;

class EnclaveProtocol : public Protocol {
private:
    Enclave *cmd;

public:
    EnclaveProtocol() {
    }

    void load_command(Enclave *c) {
        cmd = c;
    }

    virtual ~EnclaveProtocol() {
    }

    /// called when a client connects to us
    virtual bool new_incoming_connection(connection_ptr conn) {
        std::cout << "LNTeeProtocol::new_incoming_connection " << conn->str() << std::endl;

        nlohmann::json j = Wallet::Instance()->get_account()->to_json();
//        j["cmd"]= payload::MESSAGE_TYPE::SEV_CMD_ACCOUNT_RET;
        string acc = j.dump();
        DEBUG(acc.c_str());
        conn->async_write(message_ptr(new GeneralMessage(payload::MESSAGE_TYPE::SEV_CMD_ACCOUNT_RET, acc)));

        conn->set("pubkey", SEV_SOCK_TOKEN);
        return true; // returning false rejects the connection
    }

    /// called when we opened a socket to a remote servent
    virtual void new_outgoing_connection(connection_ptr conn) {
        std::cout << "LNTeeProtocol::new_outgoing_connection " << conn->str() << std::endl;
        nlohmann::json msg;
//        msg["pubkey"] = Global::pubkey_hex;           // Personal pubkey
        std::string ss = msg.dump();
        conn->async_write(message_ptr(new GeneralMessage(payload::MESSAGE_TYPE::Internet_connection, ss)));
    }

    /// called on a disconnection, for whatever reason
    virtual void connection_terminated(connection_ptr conn) {
        std::cout << "Connection terminated!" << std::endl;
    }

    /// we received a msg from this connection
    virtual void message_received(message_ptr msgp, connection_ptr conn) {
        DEBUG("");

        std::string res;
        string str = std::string(msgp->payload());
        DEBUG(str.c_str());
        str = str.substr(0, ntohl(msgp->header().length));
        DEBUG(str.c_str());

//        std::cout <<std::endl<<ntohl(msgp->header().length)<<std::endl;

        nlohmann::json j = nlohmann::json::parse(str);
//        std::cout << "Type = "<<msgp->type()<<std::endl;
        switch (msgp->type()) {
            case payload::MESSAGE_TYPE::Internet_connection:
                break;
            case payload::MESSAGE_TYPE::HOST_CMD_LOAD_CONTRACT:
                DEBUG("HOST_CMD_LOAD_CONTRACT");

                cmd->ecall_lntee_main(j["contract"].get<string>().c_str());
                break;
            case payload::MESSAGE_TYPE::Internet_connection_res:
                DEBUG("STEP2 of ECDH");
                DEBUG(msgp->payload());
                break;

            case payload::MESSAGE_TYPE::HOST_CMD_DIRECT_SEND_TX:
                DEBUG("HOST_CMD_DIRECT_SEND_TX");
//                 cmd->time_curr("DIRECT SEND INTERNET RECV");
// //                j = nlohmann::json::parse(msgp->payload());
//                 cmd->direct_recv(msgp->payload());
//                 DEBUG("");

                DEBUG("");
                cmd->ecall_lntee_direct_send(j["pubkey"].get<std::string>(), j["amt"].get<int>());
                DEBUG("");
                break;

            case payload::MESSAGE_TYPE::HOST_CMD_DIRECT_RECV_TX:
                DEBUG("HOST_CMD_DIRECT_RECV_TX");
                str = j["tx"].get<std::string>();
                cmd->ecall_lntee_direct_recv(str.c_str());
                break;

            case payload::MESSAGE_TYPE::HOST_CMD_INVOKE_SEND_TX:
                DEBUG("HOST_CMD_INVOKE_SEND_TX");
                // 2
                str = j["tx"].get<std::string>();
                DEBUG(str.c_str());
                cmd->ecall_lntee_send(str.c_str());
                DEBUG("");
                break;

            case payload::MESSAGE_TYPE::HOST_CMD_INVOKE_RECV_TX:
                DEBUG("HOST_CMD_INVOKE_RECV_TX");
                DEBUG("");
                DEBUG(j.dump().c_str());
                str = j["tx"].get<std::string>();
                cmd->ecall_lntee_recv_transaction((const unsigned char *) str.c_str());
                break;
            default: {
                std::cout << "Unhandled message type: " << msgp->type() << std::endl;
                break;
            }
        }
    }
};

#endif //LNTEE_ENCLAVE_PROTOCOL_H
