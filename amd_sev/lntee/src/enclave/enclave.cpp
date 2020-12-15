#include <enclave/enclave.h>
#include <eEVM/util.h>

using namespace std;

/**
 * Get random string
 * @param length size of the random string
 * @return
 */
std::string random_string(size_t length) {
    LOG("");
    srand(time(NULL));
    auto randchar = []() -> char {
        const char charset[] =
                "0123456789"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "abcdefghijklmnopqrstuvwxyz";
        const size_t max_index = (sizeof(charset) - 1);
        return charset[rand() % max_index];
    };
    std::string str(length, 0);
    std::generate_n(str.begin(), length, randchar);
    return str;
}

//
//int Enclave::ecall_lntee_recv_tx(const unsigned char *tx, size_t len)
//{
//    std::cout << "TX:" << tx << std::endl;
//    return -1;
//}

int Enclave::ecall_lntee_init_tee() {
    pool = new ThreadPool(40);
    LOG("");

    initial_aes();
    string hex_prikey = Wallet::Instance()->recover_account();
    if (hex_prikey.length() < 1) {
        std::string rand = random_string(512);

        // Generate prikey according to the hash of sealing result
        mbedtls_sha256(
                (unsigned char *) rand.c_str(), 512,
                (unsigned char *) &Wallet::Instance()->get_account()->prikey, 0);
        Wallet::Instance()->persist_account();
    }

    provision_ecdsa_key((char *) &Wallet::Instance()->get_account()->prikey, SECKEY_LEN);
    get_address((unsigned char *) &Wallet::Instance()->get_account()->pubkey,
                (unsigned char *) &Wallet::Instance()->get_account()->address);

    std::cout << "\n------------------------\n"
              << std::endl;

    std::cout << "ADDRESS: "
              << eevm::to_hex_string(Wallet::Instance()->get_account()->address) << std::endl;
    std::cout << "PRIKEY : "
              << eevm::to_hex_string(Wallet::Instance()->get_account()->prikey)
              << std::endl;
    std::cout << "PUBKEY :"
              << eevm::to_hex_string(Wallet::Instance()->get_account()->pubkey)
              << std::endl;

    nlohmann::json j = Wallet::Instance()->get_account()->to_json();
    string acc = j.dump();
    DEBUG(acc.c_str());

    std::cout << "\n------------------------\n"
              << std::endl;
    // For test
    Wallet::Instance()->set_balance(1000000);

    // // Copy TEE pubkey to the host
    // memcpy((void *) pubkey,
    //        lntee::to_hex((unsigned char *) &Wallet::Instance()->get_account()->pubkey, PUBKEY_LEN).c_str(),
    //        PUBKEY_LEN * 2);
    // memcpy((void *) addr,
    //        lntee::to_hex((unsigned char *) &Wallet::Instance()->get_account()->address, ADDRESS_LEN).c_str(),
    //        ADDRESS_LEN * 2);

    // initial_aes();

    return 0;
}

/**
 * Process contract function call
 *
 * @param contract_addr verify the contract addr
 * @param origin caller
 * @param tx transaction script
 * @return executing result
 */
bool
Enclave::process_contract_invoke(const unsigned char *contract_addr, eevm::Address origin, const unsigned char *tx) {
    eevm::Code c = eevm::to_bytes((char *) tx);
    return contract.invoke(origin, c);
}

/**
 * Process the msg from host
 * @param msg msg will be a json object in string format
 */
void Enclave::process_message_from_host(const unsigned char *msg) {
    if (strlen((char *) msg) < 1)
        return;

    nlohmann::json j = nlohmann::json::parse(msg);
    payload::MESSAGE_TYPE cmd = static_cast<payload::MESSAGE_TYPE>(j["cmd"].get<int>());

    string str_res;
    DEBUG(msg);
    switch (cmd) {

        case payload::MESSAGE_TYPE::HOST_CMD_LOAD_CONTRACT: {
            DEBUG("");
            string str = j["contract"].get<string>();
            const char *contract = str.c_str();
//            DEBUG(contract);
//            std::cout << j["contract"].get<string>() << std::endl;
//            auto j = nlohmann::json::parse(contract);
//            DEBUG(j.dump().c_str());
//            string jjj = j["contract"].get<string>();
//            string str = "{\"bin\":\"608060405234801561001057600080fd5b506040516104423803806104428339818101604052602081101561003357600080fd5b50516000818155338152600160205260409020556103ec806100566000396000f3fe608060405234801561001057600080fd5b506004361061007e577c01000000000000000000000000000000000000000000000000000000006000350463095ea7b3811461008357806318160ddd146100c357806323b872dd146100dd57806370a0823114610113578063a9059cbb14610139578063dd62ed3e14610165575b600080fd5b6100af6004803603604081101561009957600080fd5b50600160a060020a038135169060200135610193565b604080519115158252519081900360200190f35b6100cb6101fa565b60408051918252519081900360200190f35b6100af600480360360608110156100f357600080fd5b50600160a060020a03813581169160208101359091169060400135610200565b6100cb6004803603602081101561012957600080fd5b5035600160a060020a03166102e6565b6100af6004803603604081101561014f57600080fd5b50600160a060020a038135169060200135610301565b6100cb6004803603604081101561017b57600080fd5b50600160a060020a038135811691602001351661038c565b336000818152600260209081526040808320600160a060020a038716808552908352818420869055815186815291519394909390927f8c5be1e5ebec7d5bd14f71427d1e84f3dd0314c0f7b2291e5b200ac8c7c3b925928290030190a35060015b92915050565b60005490565b600160a060020a038316600090815260016020526040812054821180159061024b5750600160a060020a03841660009081526002602090815260408083203384529091529020548211155b156102db57600160a060020a038085166000818152600160209081526040808320805488900390559387168083528483208054880190559282526002815283822033808452908252918490208054879003905583518681529351929391927fddf252ad1be2c89b69c2b068fc378daa952ba7f163c4a11628f55a4df523b3ef9281900390910190a35060016102df565b5060005b9392505050565b600160a060020a031660009081526001602052604090205490565b3360009081526001602052604081205482116103845733600081815260016020908152604080832080548790039055600160a060020a03871680845292819020805487019055805186815290519293927fddf252ad1be2c89b69c2b068fc378daa952ba7f163c4a11628f55a4df523b3ef929181900390910190a35060016101f4565b5060006101f4565b600160a060020a0391821660009081526002602090815260408083209390941682529190915220549056fea265627a7a7231582008c04fc96fa8e6ab92ee340392e77d247192f9422e62868c068f1c074821a22c64736f6c634300050b0032\",\"hashes\":{\"allowance(address,address)\":\"dd62ed3e\",\"approve(address,uint256)\":\"095ea7b3\",\"balanceOf(address)\":\"70a08231\",\"totalSupply()\":\"18160ddd\",\"transfer(address,uint256)\":\"a9059cbb\",\"transferFrom(address,address,uint256)\":\"23b872dd\"}}";
//
//            jjj = jjj.substr(0,str.length());

//            if(jjj == str){
//                DEBUG("<><><><><><><>><><><<>><><")
//            }

//            DEBUG("");
//            nlohmann::json jj = nlohmann::json::parse(jjj.c_str());
            DEBUG("");
            ecall_lntee_main(contract);
            break;
        }
        case payload::MESSAGE_TYPE::HOST_CMD_DIRECT_RECV_TX: {
            DEBUG(msg);
            const char *k = j["pubkey"].get<std::string>().c_str();
            DEBUG("");
            int amt = j["amt"].get<int>();
//            str_res = ecall_lntee_direct_send(k, amt);
            break;
        }
        case payload::MESSAGE_TYPE::HOST_CMD_DIRECT_SEND_TX: {
            DEBUG(msg);
            string k = j["pubkey"].get<std::string>();
            DEBUG("");
            int amt = j["amt"].get<int>();
//            str_res = ecall_lntee_direct_send(k, amt);

            DEBUG("");
            break;
        }
        case payload::MESSAGE_TYPE::HOST_CMD_INVOKE_SEND_TX: {

            DEBUG("");
            const char *k = j["tx"].get<std::string>().c_str();
            ecall_lntee_send(k);

            break;
        }
        default:
            break;
    }
//    case RECV_TRANSACTION:
//        //Debug();
//        size = (int)msg[2];
//        memcpy(addr, &msg[3], size);
//        amount = (int)msg[1];
////        bal.recvAsset((char *)addr, amount);
//        break;
//    case SEND_TRANSACTION:
//        //Debug();
//        amount = (int)msg[1];
//        size = (int)msg[2];
//        memcpy(addr, &msg[3], size);
////        if (bal.try_send((char *)addr, amount))
////        {
////            tx_hex[0] = SEND_TRANSACTION;
////            tx_hex[1] = amount;
////            tx_hex[2] = strlen((char *)addr);
////            memcpy(&tx_hex[3], addr, strlen((char *)addr));
////            pool->enqueue([&](const unsigned char *tx) {
////                sign_sign((unsigned char *)tx);
////                server._send(tx, 512);
////            },
////                          tx_hex);
////        }
////        else
////        {
////            cout << "Balance not enough" << endl;
////        }
//        break;
//    case HOST_REQUEST_SEV_ACCOUNT:
//    {
//        //Debug();
//        memset(tx_hex, 0, 512);
//        tx_hex[0] = HOST_REQUEST_SEV_ACCOUNT;
//        tx_hex[1] = ADDRESS_LEN;
////        memcpy(&tx_hex[2], bal.acc.address, ADDRESS_LEN);
//
//        server._send(tx_hex, 512,libf2f::message_ptr(
//                new libf2f::GeneralMessage(payload::MESSAGE_TYPE::Direct_tranaction, lntee::to_hex(tx, 16 + 64 + 32 + 32))));
//        break;
//    }
//    case CONTRACT_MSG:
//    {
//        // msg_type 1| size 1| contract 32 | size 1| bin_sev_address 20 | size 1| script
//        //Debug();
//        eevm::Address addr;
//        memcpy((char *)&addr, (char *)(msg + 36), ADDRESS_LEN);
//        //Debug();
//        size = (int)msg[57];
//        unsigned char *script = new unsigned char[size + 1];
//        bool need_sign = false;
//        //Debug();
//        memcpy(script, &msg[58], size);
//        script[size] = '\0';
//
//        cout << "- SEV-RECV = " << lntee::to_hex(msg, 512) << endl;
//        unsigned char *tmp = new unsigned char[512];
//        memcpy(tmp, msg, 512);
//        pool->enqueue([&](eevm::Address address, unsigned char *sp, bool sig) {
//            process_contract_invoke(&tmp[2], address, sp);
//            if (sig)
//            {
//                //Debug();
//         //       sign_sign(tmp);
//                //Debug();
//                cout << "- SEV-RECV = " << lntee::to_hex(tmp, 512) << endl;
//                server._send(tmp, 512);
//            }
//        },
//                      addr, script, need_sign);
//        break;
//    }
//    default:
//        //Debug();
//        break;
//    }
}


void Enclave::ecall_lntee_main(const char *contract_definition) {
//    LOG(contract_definition);
//    DEBUG(contract_definition);
//    DEBUG("");
//    std::cout << "string len = " << strlen((char *) contract_definition) << std::endl;


//    int len = strlen((char *)contract_definition);
//    for (int i = 0; i < len; ++i) {
//        if(contract_definition[i] == str[i]){
//            printf("%d %02x %02x \n",i, contract_definition[i],str[i]);
//        }
//    }

//    DEBUG("");
//    printf("________________________");
//    printf("________________________");
//    printf("\n<><><>++ %c  ++<><><>\n",contract_definition);
//    std::cout <<""<<(char)contract_definition[0]<<std::endl;
//    string str = "{\"bin\":\"608060405234801561001057600080fd5b506040516104423803806104428339818101604052602081101561003357600080fd5b50516000818155338152600160205260409020556103ec806100566000396000f3fe608060405234801561001057600080fd5b506004361061007e577c01000000000000000000000000000000000000000000000000000000006000350463095ea7b3811461008357806318160ddd146100c357806323b872dd146100dd57806370a0823114610113578063a9059cbb14610139578063dd62ed3e14610165575b600080fd5b6100af6004803603604081101561009957600080fd5b50600160a060020a038135169060200135610193565b604080519115158252519081900360200190f35b6100cb6101fa565b60408051918252519081900360200190f35b6100af600480360360608110156100f357600080fd5b50600160a060020a03813581169160208101359091169060400135610200565b6100cb6004803603602081101561012957600080fd5b5035600160a060020a03166102e6565b6100af6004803603604081101561014f57600080fd5b50600160a060020a038135169060200135610301565b6100cb6004803603604081101561017b57600080fd5b50600160a060020a038135811691602001351661038c565b336000818152600260209081526040808320600160a060020a038716808552908352818420869055815186815291519394909390927f8c5be1e5ebec7d5bd14f71427d1e84f3dd0314c0f7b2291e5b200ac8c7c3b925928290030190a35060015b92915050565b60005490565b600160a060020a038316600090815260016020526040812054821180159061024b5750600160a060020a03841660009081526002602090815260408083203384529091529020548211155b156102db57600160a060020a038085166000818152600160209081526040808320805488900390559387168083528483208054880190559282526002815283822033808452908252918490208054879003905583518681529351929391927fddf252ad1be2c89b69c2b068fc378daa952ba7f163c4a11628f55a4df523b3ef9281900390910190a35060016102df565b5060005b9392505050565b600160a060020a031660009081526001602052604090205490565b3360009081526001602052604081205482116103845733600081815260016020908152604080832080548790039055600160a060020a03871680845292819020805487019055805186815290519293927fddf252ad1be2c89b69c2b068fc378daa952ba7f163c4a11628f55a4df523b3ef929181900390910190a35060016101f4565b5060006101f4565b600160a060020a0391821660009081526002602090815260408083209390941682529190915220549056fea265627a7a7231582008c04fc96fa8e6ab92ee340392e77d247192f9422e62868c068f1c074821a22c64736f6c634300050b0032\",\"hashes\":{\"allowance(address,address)\":\"dd62ed3e\",\"approve(address,uint256)\":\"095ea7b3\",\"balanceOf(address)\":\"70a08231\",\"totalSupply()\":\"18160ddd\",\"transfer(address,uint256)\":\"a9059cbb\",\"transferFrom(address,address,uint256)\":\"23b872dd\"}";
//
    // cout << contract_definition << endl;
//    auto json = nlohmann::json::parse(contract_definition);
//    DEBUG("");
//    std::cout <<json.dump()<< std::endl;
//    DEBUG(str.c_str());
//    DEBUG(contract_definition);

//string str = convertToString(contract_definition);
    contract.loadContract(nlohmann::json::parse(contract_definition),
                          &Wallet::Instance()->get_account()->address);
}

/**
 * Send credit directly to counterpart
 * @param pubkey
 * @param amount
 * @param tx_str
 * @param tx_size
 */
void Enclave::ecall_lntee_direct_send(string pubkey, int amt) {
//    ocall_lntee_time_log();
    DEBUG("");
    char cipher[80];
    char plain[80];
    char tx[16 * 5 + 1];
    unsigned char py[64];
    lntee::from_hex(pubkey.c_str(), (char *) py);

    if (Wallet::Instance()->direct_send(*(eevm::Pubkey *) py, 100)) {
        // 0 -15 bytes amt
        // 16 - 79 bytes local_pubkey
        memcpy((void *) tx, (char *) &amt, sizeof(int));
        memcpy((void *) (tx + 16), (char *) &Wallet::Instance()->get_account()->pubkey, PUBKEY_LEN);
#ifdef _TEST_ECDSA_

        //        char msg[1024] = "helloworldhelloworldhelloworldhelloworldhelloworldhelloworldhelloworldhelloworldhelloworldhelloworldhelloworldhelloworld";
        unsigned char output1[32];
//    char cipher[1024] = {'\0'};
//    char plain[1024] = {'\0'};
//    print_hex("Method 1", output1, sizeof output1);

//    for (int i = 0; i < 10000; i++) {
//        * \param input    buffer holding the  data
//        * \param ilen     length of the input data
//        * \param output   SHA-224/256 checksum result
//        * \param is224    0 = use SHA256, 1 = use SHA224
        mbedtls_sha256((const unsigned char *) tx, 144, output1, 0);
        ecdsa_sign(output1);
#else
        aes_encrypt((unsigned char *) tx, 5, (unsigned char *) cipher);
        aes_decrypt((char *) cipher, 5, (unsigned char *) plain);
#endif

        nlohmann::json j;
//        j["cmd"] = payload::MESSAGE_TYPE::SEV_CMD_DIRECT_SEND_TX_RES;
        j["tx"] = lntee::to_hex((const unsigned char *) tx, 16 * 5);
        j["pubkey"] = pubkey;

//        send_msg_through_sock(j);
//    }
        DEBUG("");
        router->send(SEV_SOCK_TOKEN, message_ptr(
                new GeneralMessage(payload::MESSAGE_TYPE::SEV_CMD_DIRECT_SEND_TX_RES, j.dump())));
        DEBUG("");
    } else {
        cout << "Balance not enough" << endl;
    }
}

int Enclave::ecall_lntee_direct_recv(const char *tx) {
    //DEBUG("");
    int amt = 0;
    char pubkey[PUBKEY_LEN];
    memcpy((void *) &amt, tx, sizeof(int));
    memcpy((void *) pubkey, tx + 16, PUBKEY_LEN);

    if (Wallet::Instance()->direct_receive(*(eevm::Pubkey *) pubkey, amt))
        return 0;

    return -1;
}

/**
 * Process transactions from the host, should return the signed transaction
 * @param function_call
 * @param tx_str return signed tx
 */
void Enclave::ecall_lntee_send(const char *function_call) {
    DEBUG("");
    eevm::Code script = lntee::from_hex(function_call);
    //    if (contract.invoke(Wallet::Instance()->get_account()->address, script)) {
    contract.invoke(Wallet::Instance()->get_account()->address, script);
    // DEBUG("SUCCESS");
    int size = (script.size() % 16 == 0) ? script.size() / 16 : (script.size() / 16 + 1);

    char *plain = new char[size * 16];
    char *tx_str = new char[size * 16];
    memset(plain, 0, size * 16);

    memcpy((void *) plain, (unsigned char *) &script[0], script.size());
    // DEBUG("");
    //    memcpy((void *)tx_str)

    //        char msg[1024] = "helloworldhelloworldhelloworldhelloworldhelloworldhelloworldhelloworldhelloworldhelloworldhelloworldhelloworldhelloworld";
#ifdef _TEST_ECDSA_

    unsigned char output1[32];
    ////    char cipher[1024] = {'\0'};
    ////    char plain[1024] = {'\0'};
    ////    print_hex("Method 1", output1, sizeof output1);
    ////    ocall_lntee_time_log();
    //
    ////    for (int i = 0; i < 10000; i++) {
    mbedtls_sha256_ret((unsigned char *) plain, size, output1, 0);
    ecdsa_sign(output1);
    //    }
    //        ocall_lntee_time_log();
    //    } else {
    //        DEBUG("FAILED");
    //        ocall_lntee_time_log();
    //    }
#else
    aes_encrypt((unsigned char *) plain, size, (unsigned char *) tx_str);
    aes_decrypt((char *) tx_str, size, (unsigned char *) plain);

#endif
    nlohmann::json j;
    j["tx"] = lntee::to_hex((const unsigned char *) plain, size * 16);
//    j["pubkey"] =pubkey;
    DEBUG("");
    string str = j.dump();
    DEBUG(str.c_str());

    router->send(SEV_SOCK_TOKEN, message_ptr(
            new GeneralMessage(payload::MESSAGE_TYPE::SEV_CMD_INVOKE_SEND_TX_RES, str)));
    DEBUG("");
}

int Enclave::ecall_lntee_recv_transaction(const unsigned char *transaction) {
    LOG("");
//    string str = "a9059cbb00000000000000000000000064b0ab6c0c3db74c4ba31b3d1e9eeda7831d6ba90000000000000000000000000000000000000000000000000000000000000023000000000000000000000000";
//    int len = strlen((const char *) transaction);
//    unsigned char *enc_tx = new unsigned char[len];
//    memset(enc_tx, 0, len);
//    lntee::from_hex((const char *) transaction, (char *) enc_tx);
//    eevm::Code plain;
//    plain.resize(len);
//    aes_decrypt((char *) enc_tx, len / 16, (unsigned char *) plain.data());
    eevm::Code script = lntee::from_hex((char *) transaction);
    if (contract.invoke(Wallet::Instance()->get_account()->address, script)) {
        // DEBUG("SUCCESS");
        return 0;
    } else {
        //  DEBUG("FAILED");
        return -1;
    }
}

Enclave::~Enclave() {
    // sign_close();
}