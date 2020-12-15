#include <iostream>
#include <fstream>
#include <string>
#include <eEVM/common.h>
#include <nlohmann/json.hpp>
//#include <eEVM/util.h>
#ifndef ACCOUNT_H
#define ACCOUNT_H

using namespace std;

typedef struct _Account {
    /* data */
    uint8_t prikey[SECKEY_LEN] = {0};
    unsigned char pubkey[PUBKEY_LEN];
    unsigned char address[ADDRESS_LEN];
    int balance;

    string to_string() {
        nlohmann::json j;
        j["address"] = lntee::to_hex((unsigned char *) address, ADDRESS_LEN);
        j["pubkey"] = lntee::to_hex((unsigned char *) pubkey, PUBKEY_LEN);

        return j.dump();
    }
} Acc;

void write_account(Acc *acc);

void write_seed(string seed);

string read_seed();

#endif