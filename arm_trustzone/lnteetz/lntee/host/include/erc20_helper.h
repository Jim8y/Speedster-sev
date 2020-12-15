//
// Created by compass on 9/24/19.
//

#ifndef LNTEE_ERC20_HELPER_H
#define LNTEE_ERC20_HELPER_H

#include <Helper.h>
#include <map>
#include <iostream>
#include <string>
#include <vector>
#include <eEVM/common.h>
#include <Global.h>

class erc20_helper : public Helper {
private:
    std::map<std::string, std::string> func;

public:
    erc20_helper() {
        func.insert(std::make_pair("allowance", "dd62ed3e"));
        func.insert(std::make_pair("approve", "095ea7b3"));
        func.insert(std::make_pair("balanceOf", "70a08231"));
        func.insert(std::make_pair("totalSupply", "18160ddd"));
        func.insert(std::make_pair("transfer", "a9059cbb"));
    }

    void showManu() {
    }

    std::vector<uint8_t> allowance(std::string addr, std::string addr2);

    std::vector<uint8_t> approve(std::string addr, int val);

    std::vector<uint8_t> balanceOf(std::string addr);

    std::vector<uint8_t> totalSupply();

    std::vector<uint8_t> transfer(std::string to, int value);

    std::vector<uint8_t> transferFrom(std::string from, std::string to, int value);
};

#endif //LNTEE_ERC20_HELPER_H
