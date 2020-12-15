//
// Created by compass on 9/24/19.
//

#include "erc20_helper.h"
#include <eEVM/common.h>

std::vector<uint8_t> erc20_helper::allowance(std::string addr, std::string addr2) {
    clean();
    function_call = to_bytes(func.find("allowance(address,address)")->second);
    return function_call;
}

std::vector<uint8_t> erc20_helper::approve(std::string addr, int val) {
    clean();
    function_call = to_bytes(func.find("approve(address,uint256)")->second);
    return function_call;
}

std::vector<uint8_t> erc20_helper::balanceOf(std::string addr) {
    clean();
    function_call = to_bytes(func.find("balanceOf(address)")->second);
    return function_call;
}

std::vector<uint8_t> erc20_helper::totalSupply() {
    clean();
    function_call = to_bytes(func.find("totalSupply")->second);
    return function_call;
}

std::vector<uint8_t> erc20_helper::transfer(std::string to, int value) {
    clean();
    eevm::Address _to, _from;
    function_call = to_bytes(func.find("transfer")->second);

    lntee::from_hex(to.c_str(), (char *) &_to);
    lntee::from_hex(Global::addr, (char *) &_from);

//    append_argument(function_call, _from);
    append_argument(function_call, _to);
    append_argument(function_call, value);
    function_call = to_bytes(
            "0xa9059cbb00000000000000000000000064b0ab6c0c3db74c4ba31b3d1e9eeda7831d6ba90000000000000000000000000000000000000000000000000000000000000023");
    return function_call;
}

std::vector<uint8_t> erc20_helper::transferFrom(std::string from, std::string to, int value) {
    clean();
    function_call = to_bytes(func.find("transferFrom(address,address,uint256)")->second);
    return function_call;
}