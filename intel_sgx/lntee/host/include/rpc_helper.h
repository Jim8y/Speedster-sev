//
// Created by compass on 9/15/19.
//

#ifndef LNTEE_RPC_HELPER_H
#define LNTEE_RPC_HELPER_H

#include <Helper.h>
#include <map>
#include <iostream>
#include <string>
#include <vector>
#include <eEVM/common.h>

class rpc_helper : public Helper {
private:
    std::map<std::string, std::string> func;

public:

    rpc_helper() {
        func.insert(std::make_pair("play", "93e84cd9"));
    };

    void showManu() {
    };

    std::vector<uint8_t> play();
};


#endif //LNTEE_RPC_HELPER_H
