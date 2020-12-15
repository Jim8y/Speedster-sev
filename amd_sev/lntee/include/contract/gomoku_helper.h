//
// Created by compass on 10/24/19.
//

#ifndef LNTEE_GOMOKU_HELPER_H
#define LNTEE_GOMOKU_HELPER_H

#include <contract/Helper.h>
#include <map>
#include <iostream>
#include <string>
#include <vector>
#include <eEVM/common.h>

class gomoku_helper : public Helper {
private:
    std::map<std::string, std::string> func;

public:
    gomoku_helper() {
        func.insert(std::make_pair("GameState", "3bbf41d9"));
        func.insert(std::make_pair("GetLastWinner", "f503f294"));
        func.insert(std::make_pair("MakeMove", "3c22b640"));
    }

    void showManu() {
    };

    std::vector<uint8_t> allowance(std::string addr, std::string addr2);

    std::vector<uint8_t> CanIJoin();

    std::vector<uint8_t> Disconnect();

    std::vector<uint8_t> GameState();

    std::vector<uint8_t> GetLastWinner();

    std::vector<uint8_t> GetLastWinnerAddress();

    std::vector<uint8_t> GetWinnerAddress();

    std::vector<uint8_t> JoinGame();

    std::vector<uint8_t> MakeMove(uint256_t player, uint256_t row, uint256_t col);

    std::vector<uint8_t> Player1();

    std::vector<uint8_t> Player2();

    std::vector<uint8_t> WaitingForPlayersMove();
};


#endif //LNTEE_GOMOKU_HELPER_H
