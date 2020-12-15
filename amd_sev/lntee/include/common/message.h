#include <eEVM/common.h>
#include <eEVM/simple/simpleglobalstate.h>
#include <eEVM/debug.h>
#include <map>
#include <vector>
#include <nlohmann/json.hpp>

#ifndef LNTEE_MESSAGE_H
# define LNTEE_MESSAGE_H

#  define MAX_PARTY 5

struct Environment {
    eevm::GlobalState *gs;
    const eevm::Address owner_address;
    const nlohmann::json contract_definition;
};

namespace payload {

    enum MESSAGE_TYPE {
        Internet_connection = 0,
        Internet_connection_res,
        Internet_connection_final,
        Initialize_from_blockchain,

        HOST_CMD_LOAD_CONTRACT,
        ////////////////////////////////
        // Direct transaction host as sender
        HOST_CMD_DIRECT_SEND_TX,
        SEV_CMD_DIRECT_SEND_TX_RES,

        // Direct transaction host as receiver
        HOST_CMD_DIRECT_RECV_TX,
        SEV_CMD_DIRECT_RECV_TX_RES,

        SEV_CMD_ACCOUNT_RET,

        // Contract transaction host as sender
        HOST_CMD_INVOKE_SEND_TX,
        SEV_CMD_INVOKE_SEND_TX_RES,
        // Contract transaction host as receiver
        HOST_CMD_INVOKE_RECV_TX,
        SEV_CMD_INVOKE_RECV_TX_RES,

        Register_to_blockchain,
        Update_initial_state,
        Claim_recv_asset,
        Verify_initial_state,
        Verify_initial_state_res,
        Send_host_to_enc,
        Send_transaction,
        Contract_transaction,
        Direct_tranaction,
        End_contract_instance
    };
}

//namespace lntee{
//    static std::string
//}

struct Message {
//  MESSAGE_TYPE type;
    const std::string conrtact_addr;
    const std::string instance_id;
    const eevm::Code &script;
};

// using DEP = std::vector<eevm::Pubkey>;
class Contract_Instance {
public:
    eevm::Contract_Address contract;
    int state[MAX_PARTY] = {0}; // the state of an instance is an integer array of
    // transaction count
    eevm::Instance_id instance_id; // hash value of all participants along
    // with the address of contract
    std::vector<eevm::Pubkey> keys; // This array should be sorted

    Contract_Instance() {
        // contract = {0};
        // instance_id = {0};
        // keys = DEP;
    };

    ~Contract_Instance() {};
};

// Initial state is the deposit record from the blockchain
struct Initial_State {
    std::vector<uint256_t>
            tx_list; // transactions that deposit credit into TEE account
    uint32_t init_balance; // Balance on the Blockchain
    uint32_t block_height; // Current Height of blockchain while initial.
};

#endif