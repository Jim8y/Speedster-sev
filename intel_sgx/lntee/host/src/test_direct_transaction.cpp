//
// Created by compass on 10/5/19.
//

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
#include <stdio.h>

#include <eEVM/common.h>

//#include <boost/asio.hpp>
//#include <boost/thread/thread.hpp>
//#include <boost/algorithm/string.hpp>
#include <openenclave/host.h>

#include "command.h"
#include "payload.pb.h"

#include "libf2f/router.h"
#include "libf2f/protocol.h"
#include "lntee_messages.h"
#include "lntee_protocol.h"
#include "erc20_helper.h"
//#define  TEST_TRANSACTION_THROUGHPUT
#ifdef TEST_TRANSACTION_THROUGHPUT
#define SHMSZ 27
using namespace std;

using namespace libf2f;

char c;
int shmid;
key_t key;
char *shm, *s;

static lntee::Command *cmd;
int is_master = 0;
bool isRun = true;

void shm_command()
{
    /*
     * We'll name our shared memory segment
     * "5678".
     */
    key = 5678;

    /*
     * Create the segment.
     */
    if ((shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0)
    {
        perror("shmget");
        exit(1);
    }

    /*
     * Now we attach the segment to our data space.
     */
    if ((shm = (char *)shmat(shmid, NULL, 0)) == (char *)-1)
    {
        perror("shmat");
        exit(1);
    }

    /*
     * Now put some things into the memory for the
     * other process to read.
     */
    s = shm;
}
void iorun(boost::asio::io_service *ios)
{
    ios->run();
    cout << "io ended" << endl;
}

void start_enclave(const char *enc_path)
{

    // Start the enclave
    cmd->time_log("Start to load the enclave");
    cmd->load_enclave(enc_path);
    cmd->time_log("Loaded the enclave");

    //Show the connection info
    std::cout << "Connect: "
              << Global::port << "@"
              << Global::ip << "@"
              << Global::pubkey << std::endl;
}

void command_parser(Router &r, std::string line)
{
    if (line == "quit")
        isRun = false;
    shm_command();
    vector<string> parts;
    boost::split(parts, line, boost::is_any_of(" "));

    if (parts[0] == "connect")
    {
        cmd->connect(parts[1]);
        is_master = atoi(parts[2].c_str());
    }
    else if (parts[0] == "contract" && parts.size() == 3)
    {
        if (parts[1] == "load")
        {
            cmd->load_contract(parts[2]);
        }
    }
    else if (parts[0] == "send")
    {
        DEBUG("");
        if (parts[1] == "-d")
        {
            // Master
            if (is_master != 1)
            {
                std::cout << "connect cmd not Master " << is_master << std::endl;
                if (is_master == 0)
                {
                    std::cout << "connect cmd last " << is_master << std::endl;
                    *s = 's';
                }
                else
                {
                    std::cout << "None Master start to wait" << is_master << std::endl;
                    while (*s != 's')
                        sleep(1);
                }

                cmd->direct_send(parts[2], atoi(parts[3].c_str()));
            }
        }
        else if (parts[1] == "-c")
        { // Call contract
            // Here should have contract instance id, but since this is just a demo, we fake one
            // ERC20 demo
            // send -c target_addr_hex target_pubkey_hex amount
            DEBUG("");
            erc20_helper helper;
            DEBUG("");
            std::vector<uint8_t> call = helper.transfer(parts[2], atoi(parts[4].c_str()));
            DEBUG("");
            cmd->send_contract_tx(parts[3], Global::to_hex((unsigned char *)&call[0], call.size()).c_str(),
                                  call.size());
            DEBUG("");
        }
    }
    else if (parts[0] == "quit")
        isRun = false;
}

int main(int argc, const char *argv[])
{
    if (argc < 3)
    {
        std::cout << "Error :./host [enc path] [p2p port]" << std::endl;
        return -1;
    }
    short port = atoi(argv[2]);
    Global::port = port;

    boost::asio::io_service ios;
    LNTeeProtocol p;
    string line;
    char b[1024] = {'\0'};
    INFO();
    start_enclave(argv[1]);
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

    while (isRun)
    {
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