//
// Created by compass on 9/15/19.
//

#include <contract/rpc_helper.h>

//    "checkWinner()": "ad38867e",
//    "play(string)": "718e6302",
//    "player1Choice()": "d8b1f219",
//    "player2Choice()": "0ce6e680",
//    "register(uint256)": "f207564e"

std::vector<uint8_t> rpc_helper::play() {
    function_call = to_bytes("93e84cd9");
    return function_call;
}