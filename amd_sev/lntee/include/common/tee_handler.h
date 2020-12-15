//
// Created by nuanbing on 2/3/20.
//

#ifndef OFFCHAINCONTRACT_TEE_HANDLER_H
#define OFFCHAINCONTRACT_TEE_HANDLER_H

#include <eEVM/simple/simpleglobalstate.h>

class tee_handler {
protected:
    eevm::Address sev_addr;
public:
    virtual void process_to_tee_message(const unsigned char *msg) = 0;

    virtual void process_from_tee_message(const unsigned char *msg) = 0;
};

#endif //OFFCHAINCONTRACT_TEE_HANDLER_H