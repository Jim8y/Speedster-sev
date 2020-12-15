#ifndef __F2FDEMO_PROTOCOL_H__
#define __F2FDEMO_PROTOCOL_H__

#include "libf2f/protocol.h"
#include "lntee_messages.h"
#include "libf2f/message.h"
#include "payload.pb.h"
#include "command.h"
#include <nlohmann/json.hpp>
#include "Global.h"
#include <eEVM/common.h>
#include <eEVM/debug.h>

using namespace libf2f;

class LNTeeProtocol : public Protocol {
private:
    lntee::Command *cmd;

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
    LNTeeProtocol() {
    }

    void load_command(lntee::Command *c) {
        cmd = c;
    }

    virtual ~LNTeeProtocol() {
    }

    /// called when a client connects to us
    virtual bool new_incoming_connection(connection_ptr conn) {
        std::cout << "LNTeeProtocol::new_incoming_connection " << conn->str() << std::endl;
        //        conn->async_write(message_ptr(new GeneralMessage(ORDINARY, std::string("sss"), std::string("sss"))));
        conn->set_name(conn->get("pubkey"));
        return true; // returning false rejects the connection
    }

    /// called when we opened a socket to a remote servent
    virtual void new_outgoing_connection(connection_ptr conn) {
        std::cout << "LNTeeProtocol::new_outgoing_connection " << conn->str() << std::endl;
        nlohmann::json msg;
        msg["pubkey"] = Global::pubkey;           // Personal pubkey
        std::string ss = msg.dump();
        conn->async_write(message_ptr(new GeneralMessage(payload::MESSAGE_TYPE::Internet_connection, ss)));
    }

    /// called on a disconnection, for whatever reason
    virtual void connection_terminated(connection_ptr conn) {
        std::cout << "Connection terminated!" << std::endl;
    }

    /// we received a msg from this connection
    virtual void message_received(message_ptr msgp, connection_ptr conn) {
        std::string res;
        nlohmann::json j;
        switch (msgp->type()) {
            case payload::MESSAGE_TYPE::Internet_connection:
                INFO();
                break;
            case payload::MESSAGE_TYPE::Internet_connection_res:
                DEBUG("STEP2 of ECDH");
                DEBUG(msgp->payload());
                cmd->time_curr("Receive ack from peer");
                break;
            case payload::MESSAGE_TYPE::Direct_tranaction:
                DEBUG("RECEIVE DIRECT TRANSACTION");
                cmd->time_curr("DIRECT SEND INTERNET RECV");
                cmd->direct_recv(msgp->payload());
                break;
            case payload::MESSAGE_TYPE::Contract_transaction:
                cmd->recv_contract_tx(msgp->payload());
                break;
            case payload::MESSAGE_TYPE::Internet_test:
                INFO();
                cmd->getRouter()->send(conn->name(), message_ptr(
                        new GeneralMessage(payload::MESSAGE_TYPE::Internet_test_res, "")));
                break;
            case payload::MESSAGE_TYPE::Internet_test_res:
                cmd->time_log("Receive test ack from peer");
                break;
            default: {
                std::cout << "Unhandled message type: " << msgp->type() << std::endl;
                break;
            }
        }
    }
};

#endif
