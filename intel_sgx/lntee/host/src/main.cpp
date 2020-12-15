
#include "Global.h"
#include <iostream>
#include <thread>
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <string>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <eEVM/common.h>

#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/algorithm/string.hpp>
#include <openenclave/host.h>

#include "command.h"
#include "payload.pb.h"

#include "libf2f/router.h"
#include "libf2f/protocol.h"
#include "lntee_messages.h"
#include "lntee_protocol.h"

#include "erc20_helper.h"
#include "gomoku_helper.h"
#include "rpc_helper.h"

#define HOST_MAIN
#ifdef HOST_MAIN

using namespace std;

using namespace libf2f;

static lntee::Command *cmd;

bool isRun = true;

void iorun(boost::asio::io_service *ios) {
    ios->run();
    cout << "io ended" << endl;
}

void start_enclave(const char *con_path, const char *enc_path) {

    // Start the enclave
    cmd->time_log("Start to load the enclave");
    cmd->load_enclave(con_path, enc_path);
    cmd->time_log("Loaded the enclave");

    //Show the connection info
    std::cout << "Connect: "
              << Global::port << "@"
              << Global::ip << "@"
              << Global::pubkey << std::endl;
}

void command_parser(Router &r, std::string line) {
    if (line == "quit")
        isRun = false;

    vector<string> parts;
    boost::split(parts, line, boost::is_any_of(" "));
    if (parts[0] == "connect" && parts.size() == 2) {
        cmd->connect(parts[1]);
    } else if (parts[0] == "contract" && parts.size() == 3) {
//        if (parts[1] == "load") {
//            cmd->load_contract(parts[2]);
//        }
    } else if (parts[0] == "send") {
        DEBUG("");
        if (parts[1] == "-d") {
            // target is the pubkey
            cmd->direct_send(parts[2], atoi(parts[3].c_str()));
        } else if (parts[1] == "-c") { // Call contract
            // Here should have contract instance id, but since this is just a demo, we fake one
            // ERC20 demo
            // send -c target_addr_hex target_pubkey_hex amount
#ifdef _CONTRACT_GOMOKU_
            gomoku_helper helper;
            std::vector<uint8_t> call = helper.MakeMove(1, 1, 1); //
#endif

#ifdef _CONTRACT_ERC20_
            erc20_helper helper;
            std::vector<uint8_t> call = helper.transfer(parts[3], 1);
#endif

#ifdef _CONTRACT_RPC_
            rpc_helper helper;
            std::vector<uint8_t> call = helper.play();
#endif
            cmd->send_contract_tx(parts[3], Global::to_hex((unsigned char *) &call[0], call.size()).c_str(),
                                  call.size());
            DEBUG("");
        }
    } else if (parts[0] == "quit")
        isRun = false;
}

int main(int argc, const char *argv[]) {
    if (argc < 4) {
//        std::cout << argv[1] <<std::endl;
        std::cout << "Error :./host [root path of contract] [enc path] [p2p port]" << std::endl;
        return -1;
    }

    string ip = argv[3];
    Global::ip = ip;

    short port = atoi(argv[4]);
    Global::port = port;

    boost::asio::io_service ios;
    LNTeeProtocol p;
    string line;
    char b[1024] = {'\0'};
    INFO();
    start_enclave(argv[1], argv[2]);
    DEBUG("");

    boost::shared_ptr<boost::asio::ip::tcp::acceptor> accp(
            new boost::asio::ip::tcp::acceptor(ios, boost::asio::ip::tcp::endpoint(
                    boost::asio::ip::tcp::v4(),
                    port)));

    Router r(std::move(accp), &p);
    cmd = new lntee::Command(&r);
    p.load_command(cmd);
    boost::thread
            t(boost::bind(&iorun, &ios));

    while (isRun) {
        cout << "# " << flush;
        cin.getline(b, 1024);
        line = string(b);
        command_parser(r, line);
    }

    r.stop();
    t.join();

    return 0;
}

#endif