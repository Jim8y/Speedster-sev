#include "eEVM/common.h"
#include "eEVM/simple/simpleglobalstate.h"

#include <map>
#include <vector>

#ifndef LNTEE_MESSAGE_H
#  define LNTEE_MESSAGE_H

#  define MAX_PARTY 5
struct Environment {
    eevm::GlobalState *gs;
    const eevm::Address owner_address;
    const nlohmann::json contract_definition;
};

enum COMMAND {
    LOAD_CONTRACT = 1,
    RECV_TRANSACTION,
    SEND_TRANSACTION
};


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
    std::vector <eevm::Pubkey> keys; // This array should be sorted

    Contract_Instance() {
        // contract = {0};
        // instance_id = {0};
        // keys = DEP;
    };

    ~Contract_Instance() {};
};

// Initial state is the deposit record from the blockchain
struct Initial_State {
    std::vector <uint256_t>
            tx_list; // transactions that deposit credit into TEE account
    uint32_t init_balance; // Balance on the Blockchain
    uint32_t block_height; // Current Height of blockchain while initial.
};

#endif