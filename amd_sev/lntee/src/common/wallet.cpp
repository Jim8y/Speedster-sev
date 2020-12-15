//
// Created by nuanbing on 5/23/19.
//

#include <common/wallet.h>
#include <eEVM/common.h>
#include <time.h>

Wallet *Wallet::m_pInstance = NULL;

Wallet *Wallet::Instance() {

    if (!m_pInstance)   // Only allow one instance of class to be generated.
        m_pInstance = new Wallet;
    return m_pInstance;
}

Wallet::Wallet() {
    this->account = new eevm::SimpleAccount();
}

Wallet::~Wallet() {}

void Wallet::persist_account() {
    std::ofstream myfile;
    myfile.open("account.txt");
    myfile << lntee::to_hex((unsigned char *) &Wallet::Instance()->get_account()->prikey, SECKEY_LEN) << std::endl;
    myfile.close();
}

string Wallet::recover_account() {

    string seed = "";
    ifstream myfile;
    if (myfile.good()) {
        myfile.open("account.txt");
        myfile >> seed;
        myfile.close();

        lntee::from_hex((const char *) seed.c_str(), (char *) &Wallet::Instance()->get_account()->prikey);

        return seed;
    } else return seed;
}


bool Wallet::in_cache(std::string addr) {
    return false;
}

void Wallet::set_balance(long bal) {
    this->valid_balance = 1000000000;
}

long Wallet::get_balance() {
    uint32_t sum_locked = 0;
    uint32_t sum_spent = 0;

    for (std::map<eevm::Instance_id, uint32_t>::iterator it = lock_list.begin();
         it != lock_list.end();
         ++it)
        sum_locked += it->second;

    for (std::map<eevm::Instance_id, uint32_t>::iterator it = spent.begin();
         it != spent.end();
         ++it)
        sum_spent += it->second;

    if (
            (this->account->get_balance() - sum_locked - sum_spent) ==
            this->valid_balance)
        return this->valid_balance;
    else
        return -1;
}

void Wallet::initialize_state(
        std::vector<uint256_t> tx_list, uint256_t amt, uint32_t block_height) {}

bool Wallet::direct_receive(eevm::Pubkey &pubkey, int amt) {
    if (amt <= 0)
        return false;

    std::map<eevm::Pubkey, uint32_t>::iterator it = receive.find(pubkey);

    if (it != receive.end())
        it->second += amt;
    else
        receive.insert(std::make_pair(pubkey, amt));
    return true;
}

bool Wallet::lock_credit(eevm::Instance_id &instance_id, uint32_t amt) {
    if (this->valid_balance < amt || amt <= 0)
        return false;
    else {
        this->valid_balance -= amt;

        // Record the transaction
        std::map<eevm::Pubkey, uint32_t>::iterator i = lock_list.find(instance_id);

        if (i == lock_list.end())
            lock_list.insert(std::make_pair(instance_id, amt));
        else
            i->second += amt;

        return true;
    }
}

bool Wallet::direct_send(eevm::Pubkey &pubkey, int amt) {

//    this->valid_balance = this->valid_balance + amt + 10000;
    DEBUG("")
    if (this->valid_balance < amt || amt <= 0)
        return false;
    else {
//        std::cout<< "valid : " <<this->valid_balance<<" amt : "<<amt<<std::endl;
        DEBUG("")
        this->valid_balance -= amt;
        DEBUG("")
        // Record the transaction
        std::map<eevm::Pubkey, uint32_t>::iterator i = spent.find(pubkey);
        if (i == spent.end())
            spent.insert(std::make_pair(pubkey, amt));
        else
            i->second += amt;
        DEBUG("")
        return true;
    }
}

/**
 * End a contract instacne, the locked credits should be moved into received
 * @param instance_id the instance id of contract
 * @param amt: the amount of credits to be locked
 */
bool Wallet::end_contract_instance(eevm::Instance_id &instance_id, uint32_t amt) {
    // Part that originally comes from the current TEE account will be returned to
    // the balance.
    std::map<eevm::Pubkey, uint32_t>::iterator it = lock_list.find(instance_id);
    if (it == lock_list.end())
        return false;

    else {
        if (it->second <= amt) {
            // If the received amt is greater than the locked credit
            std::map<eevm::Pubkey, uint32_t>::iterator i = receive.find(instance_id);
            if (i == receive.end())
                receive.insert(std::make_pair(instance_id, amt - it->second));
            else
                i->second += (amt - it->second);

            // All the locked credit will be returned
            this->valid_balance += it->second;
        } else {
            // Only the returned credit will be valid, the rest will be marked spent
            this->valid_balance += amt;

            // The rest of the credit will be marked as spent
            this->direct_send(instance_id, it->second - amt);
        }

        // Remove from the lock_list
        lock_list.erase(it);
        return true;
    }
}
