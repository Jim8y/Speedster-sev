//
// Created by nuanbing on 1/31/20.
//

#ifndef LNTEE_LNTEE_PROTOCOL_H
#define LNTEE_LNTEE_PROTOCOL_H

#include "libf2f/protocol.h"
#include "libf2f/message.h"
#include <common/message.h>
#include "command.h"
#include <nlohmann/json.hpp>
#include "Global.h"
#include <eEVM/common.h>
#include <cstdlib>

using namespace libf2f;

class LNTeeProtocol : public Protocol {
private:
    lntee::Command *cmd;
    int count = 10000;
    int r = 0;
    string tg = "0x9cf2c942bc2dddba9a9b5cda4d449330c7fbd90271916e65aac0e5a353a5c61d8b20f3a2d51f2150bca091ea81a79a7fcd598ad9ff3281a8b1fbafd6bf8ae690";


#ifdef _CONTRACT_GOMOKU_
    char const *call = "0x3c22b640000000000000000000000000000000000000000000000000000000000000000100000000000000000000000000000000000000000000000000000000000000010000000000000000000000000000000000000000000000000000000000000001";
#endif

#ifdef _CONTRACT_ERC20_
    char const *call = "0xa9059cbb00000000000000000000000064b0ab6c0c3db74c4ba31b3d1e9eeda7831d6ba90000000000000000000000000000000000000000000000000000000000000023";
#endif

#ifdef _CONTRACT_RPC_
    char const *call = "0x93e84cd9";
#endif


public:
    LNTeeProtocol() = default;

    void load_command(lntee::Command *c) {
        cmd = c;
    }

    virtual ~LNTeeProtocol() {
    }

    /// called when a client connects to us
    virtual bool new_incoming_connection(connection_ptr conn) {
        std::cout << "LNTeeProtocol::new_incoming_connection " << conn->str() << std::endl;
        //        conn->async_write(message_ptr(new GeneralMessage(ORDINARY, std::string("sss"), std::string("sss"))));

        return true; // returning false rejects the connection
    }

    /// called when we opened a socket to a remote servent
    virtual void new_outgoing_connection(connection_ptr conn) {
        std::cout << "LNTeeProtocol::new_outgoing_connection " << conn->str() << std::endl;

        nlohmann::json msg;
        msg["pubkey"] = Global::pubkey_hex;           // Personal pubkey
        std::string ss = msg.dump();

        if (ss.length() < 32) {
            return;
        }
        DEBUG(ss.c_str());
        conn->async_write(message_ptr(new GeneralMessage(payload::MESSAGE_TYPE::Internet_connection, ss)));
    }

    /// called on a disconnection, for whatever reason
    virtual void connection_terminated(connection_ptr conn) {
        std::cout << "Connection terminated!" << std::endl;
    }

    /// we received a msg from this connection
    virtual void message_received(message_ptr msgp, connection_ptr conn) {
        std::string res;

        string str = std::string(msgp->payload());
        DEBUG(str.c_str());
        str = str.substr(0, ntohl(msgp->header().length));
        DEBUG(str.c_str());

        nlohmann::json j = nlohmann::json::parse(str);
        switch (msgp->type()) {
            case payload::MESSAGE_TYPE::Internet_connection:
                break;
            case payload::MESSAGE_TYPE::Internet_connection_res:
                DEBUG("STEP2 of ECDH");
                DEBUG(msgp->payload());
                break;
            case payload::MESSAGE_TYPE::HOST_CMD_DIRECT_SEND_TX:
                DEBUG("RECEIVE DIRECT TRANSACTION");
//                cmd->time_curr("DIRECT SEND INTERNET RECV");
//                j = nlohmann::json::parse(msgp->payload());
                cmd->direct_recv(str.c_str());
                DEBUG("");
                break;
            case payload::MESSAGE_TYPE::Contract_transaction:
                cmd->recv_contract_tx(str.c_str());
                break;
            case payload::MESSAGE_TYPE::SEV_CMD_ACCOUNT_RET:
                DEBUG("SEV ACCOUNT RET");
                DEBUG(msgp->payload());
                cmd->sev_handler.connect_sev_enclave(msgp->payload());

                cmd->load_contract(_CONTRACT_);
                break;
            case payload::MESSAGE_TYPE::SEV_CMD_DIRECT_SEND_TX_RES: {
                cmd->get_router()->send(tg, message_ptr(
                        new GeneralMessage(payload::MESSAGE_TYPE::HOST_CMD_DIRECT_SEND_TX, str)));

                if (count == 0) {
                    cmd->time_log("END OF THE GAME");
//                    count = 10000;
                    break;
                }
//                if(count == 10000){
//                    srand (time(NULL));
//                    r = rand() % 10 + 1;
//                }
                cmd->direct_send(tg, (count + r) % 10 + 1);
                count--;
                break;
            }
            case payload::MESSAGE_TYPE::SEV_CMD_INVOKE_SEND_TX_RES: {
                // 3
                DEBUG(str.c_str());

                cmd->get_router()->send(tg, message_ptr(
                        new GeneralMessage(payload::MESSAGE_TYPE::HOST_CMD_INVOKE_SEND_TX, str)));
                if (count == 0) {
                    cmd->time_log("END OF THE GAME");
//                    count = 10000;
                    break;
                }
//                if(count == 10000){
//                    srand (time(NULL));
//                    r = rand() % 10 + 1;
//                }
                cmd->send_contract_tx(tg, call, 10);
                count--;
                break;
            }

            case payload::MESSAGE_TYPE::HOST_CMD_INVOKE_SEND_TX: {

                DEBUG(str.c_str());
                cmd->recv_contract_tx(str.c_str());
                break;
            }

            default: {
                std::cout << "Unhandled message type: " << msgp->type() << std::endl;
                break;
            }
        }
    }
};

#endif //LNTEE_LNTEE_PROTOCOL_H
