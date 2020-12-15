#include <enclave/enclave.h>
#include <socket/config.h>
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include "libf2f/router.h"

#include <enclave/enclave_protocol.h>

using namespace std;

using namespace libf2f;

static Enclave *enc;


int main() {
    boost::asio::io_service ios;
    EnclaveProtocol p;

    /** Start up the host server to listen to the peer connection */
    boost::shared_ptr<boost::asio::ip::tcp::acceptor> accp(
            new boost::asio::ip::tcp::acceptor(ios, boost::asio::ip::tcp::endpoint(
                    boost::asio::ip::tcp::v4(),
                    PORT_SEV)));

    Router r(std::move(accp), &p);
    enc = new Enclave(&r);

    p.load_command(enc);

    enc->ecall_lntee_init_tee();

    ios.run();
    cout << "Server started" << endl;
    return 0;
}