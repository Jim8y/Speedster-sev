//
// Created by compass on 10/24/19.
//

#include <gomoku_helper.h>

std::vector<uint8_t> gomoku_helper::GetLastWinnerAddress() {

    return function_call;
}

std::vector<uint8_t> gomoku_helper::MakeMove(uint256_t player, uint256_t row, uint256_t col) {
    clean();
    function_call = to_bytes(func.find("MakeMove")->second);
    append_argument(player);
    append_argument(row);
    append_argument(col);

    return function_call;
}