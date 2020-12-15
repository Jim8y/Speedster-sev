//
// Created by nuanbing on 5/23/19.
//

#include <enclave/account.h>

void write_account(Acc *acc) {
    ofstream myfile;
    myfile.open("account.txt");
    myfile << lntee::to_hex(acc->address, ADDRESS_LEN) << std::endl;
    myfile << lntee::to_hex(acc->prikey, SECKEY_LEN) << std::endl;
    myfile << lntee::to_hex(acc->pubkey, PUBKEY_LEN) << std::endl;
    myfile.close();
}


void write_seed(string seed) {
    ofstream myfile;
    myfile.open("seed.txt");
    myfile << seed;
    myfile.close();
}

string read_seed() {
    string seed;
    ifstream myfile;
    if (myfile.good()) {
        myfile.open("seed.txt");
        myfile >> seed;
        myfile.close();
        return seed;
    } else return seed;

}
