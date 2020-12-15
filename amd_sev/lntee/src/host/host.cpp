#include <host/host.h>

#include "eEVM/util.h"
#include <time.h>

int test_count = 0;

/**
 * Test contract executing 
 */
void load_contract() {
//    unsigned char hash[32];
//    cout << "*Loading contract..." << endl;
//    string contract_path = "sync.json"; //"ERC20_combined.json"; //
//
//    // Open the contract definition file
//    std::ifstream contract_fstream(contract_path);
//    if (!contract_fstream) {
//        throw std::runtime_error(
//                std::string("Unable to open contract definition file ") + contract_path);
//    }
//
//    std::string str(
//            (std::istreambuf_iterator<char>(contract_fstream)),
//            std::istreambuf_iterator<char>());
//    unsigned char tx[1024 * 4] = {'\0'};
//    tx[0] = LOAD_CONTRACT;
//    tx[1] = str.length();
//    memcpy(&tx[2], str.c_str(), str.length());
//    sev_enclave_socket._send(tx, 1024 * 4);
//
//    mbedtls_sha256((const unsigned char *) str.c_str(), str.length(), hash, 0);
//    cout << "*Contract loaded" << endl;

//    contractSync.contract_main(&sev_enclave_socket, (unsigned char *) hash);
}

/**
 * Test transaction 
 */
void test_send_tx(int test_scale) {

//    const size_t tx_size = 512;
//    unsigned char tx[tx_size] = {'0'};
//
//    eevm::Address target;
//
//    std::cout << "Input the target address and amount of coin to send:"
//              << std::endl;
//    int amount = 0;
//    string addr = "";
//    std::cin >> addr >> amount;
//
//    lntee::from_hex((const char *) addr.c_str(), (char *) &target);
//    tx[0] = SEND_TRANSACTION;
//    tx[1] = amount;
//    tx[2] = addr.length();
//    memcpy(&tx[3], addr.c_str(), addr.length());
//    time_t seconds = time(NULL);
//    cout << "Current time = " << (long long) seconds << endl;
//    for (int i = 0; i < test_scale; ++i) {
//        usleep(1000);
//        sev_enclave_socket._send(tx, tx_size);
//    }
}

/**
 * Start the host server 
 */
void start_host_server(int node_id) {
//    server._init(resv_call_back, node_id + PORT_BASE);
//    server._bind();
//    server._listen();
}

///**
// * Start host client to connect other nodes
// */
//void start_host_client(int node_id) {
//
//    try {
//        clients[0]._init(resv_call_back, PORT_BASE + (node_id + 1) % 3);
//        clients[0]._connect();
//
//        clients[1]._init(resv_call_back, PORT_BASE + (node_id + 2) % 3);
//        clients[1]._connect();
//    }
//    catch (const exception &e) {
//        std::cout << e.what();
//    }
//}

/**
 * Connect to the SEV enclave
 */
void start_sev_enclave(const char *enclave_ip) {
//    try {
//        /** connect to the sev enclave */
//        sev_enclave_socket._init(sev_call_back, PORT_SEV, enclave_ip);
//        sev_enclave_socket._connect();
//    }
//    catch (exception &e) {
//        cout << e.what() << endl;
//        return;
//    }
//    unsigned char tx[1];
//    tx[0] = HOST_REQUEST_SEV_ACCOUNT;
//
//    /** request the address of sev account */
//    sev_enclave_socket._send(tx, strlen((char *) tx));
}

/**
 * Receive msg from peer nodes
 * Forward this message to SEV to process it.
 */
void resv_call_back(const unsigned char *tx, size_t len) {
//    unsigned char *tx_temp = const_cast<unsigned char *>(tx);
//    //Debug();
////    cout << "- Recv : " << lntee::to_hex(tx_temp, 512) << endl;
//    if ((int) tx_temp[0] != CONTRACT_MSG)
//        tx_temp[0] = RECV_TRANSACTION;
//    thread t([tx_temp]() {
//        if (strlen((char *) tx_temp) < 1)
//            return;
//        //Debug();
//        sev_enclave_socket._send(tx_temp, 512);
//    });
//    t.join();
}


/**
 * Receive msg from SEV
 * Broadcast this information to the whole group
 */
void sev_call_back(const unsigned char *tx, size_t len) {
//    //Debug();
////    cout << "- Recv : " << lntee::to_hex(tx, 512) << endl;
//    thread t([tx]() {
//        //Debug();
//        if (strlen((char *) tx) < 2)
//            return;
//
//        //Debug();
//        process_sev_message(tx);
//    });
//    t.join();
}

//void process_sev_message(const unsigned char *msg) {
//    int cmd = (int) msg[0];
//    //Debug();
////    cout << "- SEND : " << lntee::to_hex(msg, 512) << endl;
//    switch (cmd) {
//        case HOST_REQUEST_SEV_ACCOUNT:
////            cout << "SEV account : " << lntee::to_hex((unsigned char *) &msg[2], ADDRESS_LEN) << endl;
////            memcpy((unsigned char *) &bin_sev_address, &msg[2], ADDRESS_LEN);
////            contractSync.load_b_sev_address((unsigned char *) &bin_sev_address);
//            break;
//
//        default:
//            //Debug();
////            cout << "- SEND : " << lntee::to_hex(msg, 512) << endl;
//            broad_cast(msg);
//            break;
//    }
//}

/**
 * Broad cast message to the group
 * @param msg
 */
void broad_cast(const unsigned char *msg) {
//    cout << "Broad cast message" << endl;
//    //Debug();
////    cout << "- SEND : " << lntee::to_hex(msg, 512) << endl;
//    thread t([msg]() {
//        if (strlen((char *) msg) < 1)
//            return;
//        //Debug();
////        cout << "- SEND : " << lntee::to_hex(msg, 512) << endl;
//        clients[0]._send(msg, 512);
//        clients[1]._send(msg, 512);
//    });
//    t.join();
}

/**
 * Send message to specific target
 * @param node_id
 * @param msg
 */
void send_to(const int node_id, const unsigned char *msg) {
//    thread t([node_id, msg]() {
//        if (strlen((char *) msg) < 1)
//            return;
//
//        clients[node_id]._send(msg, 512);
//    });
//    t.join();
}

void contract_menu() {
//    load_contract();
}