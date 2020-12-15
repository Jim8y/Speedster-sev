//
// Created by nuanbing on 5/30/19.
//

#ifndef EVM_CONTRACT_H
#define EVM_CONTRACT_H

#include <socket/sock.h>

class Contract_Base {

protected:

    unsigned char contract_address[32];
    unsigned char bin_sev_address[20];
    sock *sev_client;

public:

    virtual void load_b_contract_address(const unsigned char *addr) = 0;

    virtual void load_b_sev_address(const unsigned char *addr) = 0;

    /**
     * Enter the contract
     * @param broad_cast_call_back
     */
    virtual void contract_main(sock *sev, unsigned char *contract_addr) = 0;

    /**
     * Receive message from peer node
     * Routed by tester message processor
     * @param msg
     */
    virtual void message_processor(const unsigned char *msg) = 0;

};

#endif //EVM_CONTRACT_H
