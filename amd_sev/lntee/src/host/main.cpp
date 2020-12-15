#include <host/host.h>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <string>
#include <eEVM/common.h>
#include <host/sev_handler.h>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/algorithm/string.hpp>

#include <host/command.h>

#include "libf2f/router.h"
#include <host/lntee_protocol.h>

#include <contract/erc20_helper.h>
#include <contract/gomoku_helper.h>
#include <contract/rpc_helper.h>

using namespace std;

using namespace libf2f;

static lntee::Command *cmd;

bool isRun = true;

void iorun(boost::asio::io_service *ios) {
    ios->run();
    cout << "io ended" << endl;
}

//void start_enclave(const char *enc_path) {
//
//    // Start the enclave
//    cmd->time_log("Start to load the enclave");
//    cmd->load_enclave(enc_path);
//    cmd->time_log("Loaded the enclave");
//
//    //Show the connection info
//    std::cout << "Connect: "
//              << Global::port << "@"
//              << Global::ip << "@"
//              << Global::pubkey_hex << std::endl;
//}

void command_parser(Router &r, std::string line) {
    if (line == "quit")
        isRun = false;
//    string target = "0x9cb7efb8881d42071935df9ded99d235570557ead9058a95324a87f1ac0e95678e2c5685e261087fec0b747516e67e9db5950a67d0b22d09c8c786404893ea49";
    vector<string> parts;
    boost::split(parts, line, boost::is_any_of(" "));
    if (parts[0] == "connect") {

//        cmd->connect(parts[1]);
        string str = "10001@127.0.0.1@0x9cf2c942bc2dddba9a9b5cda4d449330c7fbd90271916e65aac0e5a353a5c61d8b20f3a2d51f2150bca091ea81a79a7fcd598ad9ff3281a8b1fbafd6bf8ae690";
        cmd->connect(str);
    } else if (parts[0] == "contract" && parts.size() == 3) {
        if (parts[1] == "load") {
//            cmd->load_contract(parts[2]);
//            cmd->sev_handler.host_cmd_load_contract(parts[2]);
        }
    } else if (parts[0] == "send") {
        DEBUG("");
        string str = "0x9cf2c942bc2dddba9a9b5cda4d449330c7fbd90271916e65aac0e5a353a5c61d8b20f3a2d51f2150bca091ea81a79a7fcd598ad9ff3281a8b1fbafd6bf8ae690";

        if (parts[1] == "-d") {
            cmd->time_log("Start the direct  send");
            cmd->direct_send(str, 1);//(parts[2], atoi(parts[3].c_str()));
        } else if (parts[1] == "-c") { // Call contract
            string addr = "0x22926ae73c074a710075bef6db7b4e723a313671";
            // send -c target_addr_hex target_pubkey_hex amount

#ifdef _CONTRACT_GOMOKU_
            gomoku_helper helper;
            std::vector<uint8_t> call = helper.MakeMove(1, 1, 1); //
#endif

#ifdef _CONTRACT_ERC20_
            erc20_helper helper;
            std::vector<uint8_t> call = helper.transfer(addr, 1);
#endif

#ifdef _CONTRACT_RPC_
            rpc_helper helper;
            std::vector<uint8_t> call = helper.play();
#endif

            cmd->time_log("Start the contract  send");

            cmd->send_contract_tx(str,
                                  eevm::to_hex_string(call).c_str(),
                                  call.size());
//            cmd->time_log("Start the contract  send END");

            DEBUG("");
        }
    } else if (parts[0] == "quit")
        isRun = false;
}


int main(int argc, const char *argv[]) {

    if (argc < 3) {
        std::cout << "Error :./host [enc ip] [p2p port]" << std::endl;
        return -1;
    }

    /** Get the host service port */
    short port = atoi(argv[2]);
    Global::port = port;

    boost::asio::io_service ios;
    LNTeeProtocol p;
    string line;
    char b[1024] = {'\0'};
    /** Start up the host server to listen to the peer connection */
    boost::shared_ptr<boost::asio::ip::tcp::acceptor> accp(
            new boost::asio::ip::tcp::acceptor(ios, boost::asio::ip::tcp::endpoint(
                    boost::asio::ip::tcp::v4(),
                    port)));
    DEBUG("");
    Router r(std::move(accp), &p);
    cmd = new lntee::Command(&r);
    p.load_command(cmd);

    INFO();
    cmd->time_log("Start to load enclave");
    //Global::sev_handler = new SEV_Handler();
    /** Get the enclave ip and connect */
//    SEV_Handler::connect_sev_enclave((const char *)argv[1],sev_call_back);
    DEBUG("");
//    cmd->sev_handler.connect_sev_enclave((const char *) argv[1]);
//    <port>@<ip>@<pubkey>
    string sev;
    sev += std::to_string(PORT_SEV) + "@";
    sev += (const char *) argv[1];
    sev += "@";
    sev += SEV_SOCK_TOKEN;
    cmd->connect(sev);
    DEBUG("");

    boost::thread
            t(boost::bind(&iorun, &ios));
    DEBUG("");
//    cmd->sev_handler.host_cmd_load_contract("ERC20");
//    command_parser(r,"10000@127.0.0.1@0x9cb7efb8881d42071935df9ded99d235570557ead9058a95324a87f1ac0e95678e2c5685e261087fec0b747516e67e9db5950a67d0b22d09c8c786404893ea49");
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
