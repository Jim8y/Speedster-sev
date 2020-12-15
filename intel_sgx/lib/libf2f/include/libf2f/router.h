#ifndef __LIBF2F_ROUTER_H_
#define __LIBF2F_ROUTER_H_

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>
#include "boost/lambda/lambda.hpp"
#include <boost/lambda/if.hpp>
#include <iostream>
#include <vector>
#include <map>
#include "libf2f/message.h"

namespace libf2f {

    class Protocol;

    class Connection;

/// aka servent - responsible for managing connections
    class Router {
    public:
        /// Constructor opens the acceptor and starts waiting for the first
        /// incoming connection.
        /// client code should create acceptor like so:
        /// acceptor(io_service,
        ///          boost::asio::ip::tcp::endpoint(
        ///             boost::asio::ip::tcp::v4(), port) )
        Router(boost::shared_ptr<boost::asio::ip::tcp::acceptor> accp,
               Protocol *p)
                : m_acceptor(accp),
                  m_protocol(p),
                  seen_connections(0) {
            start();
        }

        void start();

        /// how a new Connection is prepped:
        connection_ptr create_connection();

        /// lamest uuid generator ever, please supply your own.
        std::string lame_uuid_gen();

        std::string gen_uuid();

        /// calls io_service::stop on acceptor.
        void stop();

        /// Handle completion of a accept operation.
        void handle_accept(const boost::system::error_code &e, connection_ptr conn);

        /// Connect out to a remote Servent:
        void connect_to_remote(boost::asio::ip::tcp::endpoint &endpoint);

        void
        connect_to_remote(boost::asio::ip::tcp::endpoint &endpoint, const std::map<std::string, std::string> &props);

        /// Handle completion of a connect operation.
        void handle_connect(const boost::system::error_code &e,
                            boost::asio::ip::tcp::endpoint &endpoint,
                            connection_ptr conn);

        /// connection terminated for any reason
        void connection_terminated(connection_ptr conn);

        /// Default message recvd callback
        void message_received(message_ptr msgp, connection_ptr conn);

        /// apply function to all registered connections
        void foreach_conns(boost::function<void(connection_ptr)>);

        /// apply function to all registered connections *except* conn
        void foreach_conns_except(boost::function<void(connection_ptr)> fun, connection_ptr conn);

        /// send msg to all registered connections
        void send_all(message_ptr msgp);

        std::string connections_str();

        std::vector<std::string> get_connected_names();

        connection_ptr get_connection_by_name(const std::string &name);

        void send(std::string target, message_ptr msg);

    private:
        /// Router keeps track of connections:
        void register_connection(connection_ptr conn);

        void unregister_connection(connection_ptr conn);

        /// all connections:
        std::vector<connection_ptr> m_connections;

        // Here we use hex form of pubkey as the key
        std::map<std::string, connection_ptr> conn_list;

        boost::mutex m_connections_mutex; // protects connections

        /// The acceptor object used to accept incoming socket connections.
        boost::shared_ptr<boost::asio::ip::tcp::acceptor> m_acceptor;

        /// protocol implementation
        Protocol *m_protocol;
        /// thread that enforces flow-control and sends outgoing msgs
        boost::thread m_dispatch_thread;

        /// misc stats:
        unsigned int seen_connections; // num incoming connections accepted
    };

} // namespace libf2f

#endif
