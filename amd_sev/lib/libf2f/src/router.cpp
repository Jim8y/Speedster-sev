#include "libf2f/router.h"
#include "libf2f/connection.h"
#include "libf2f/protocol.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread.hpp>
#include <boost/foreach.hpp>

namespace libf2f {

    using namespace std;

    void Router::start() {
        cout << "OK" << endl;
        this->m_protocol->set_router(this);
        // Start an accept operation for a new connection.
        connection_ptr _conn = create_connection();
        m_acceptor->async_accept(_conn->socket(),
                                 boost::bind(&Router::handle_accept, this,
                                             boost::asio::placeholders::error, _conn));
    }

    connection_ptr Router::create_connection() {
        if (m_acceptor == NULL)
            cout << "m_acceptor empty" << endl;
        boost::asio::io_service *io_s = &m_acceptor->get_io_service();
        std::shared_ptr<boost::asio::ip::tcp::socket> soc = std::make_shared<boost::asio::ip::tcp::socket>(*io_s);
        connection_ptr ptr = boost::make_shared<Connection>(soc, this);
        return ptr;
    }

    void Router::stop() {
        while (m_connections.size()) {
            // this removes the connection from m_connections:
            m_connections.front()->fin();
        }
    }

    void Router::connection_terminated(connection_ptr conn) {
        unregister_connection(conn);
        m_protocol->connection_terminated(conn);
    }

/// Handle completion of a accept operation.
    void Router::handle_accept(const boost::system::error_code &e, connection_ptr conn) {
        if (e) {
            // Log it and return. Since we are not starting a new
            // accept operation the io_service will run out of work to do and the
            // Servent will exit.
            std::cerr << e.message() << std::endl;
            return;
        }
        if (!m_protocol->new_incoming_connection(conn)) {
            // cout << "Rejecting connection " << conn->str() << endl;
            // don't register it (so it autodestructs)
        } else {
            register_connection(conn);
            conn->async_read();
        }

        // Start an accept operation for a new connection.
        connection_ptr _conn = create_connection();

        m_acceptor->async_accept(_conn->socket(),
                                 boost::bind(&Router::handle_accept, this,
                                             boost::asio::placeholders::error, _conn));
    }

    /**
     * Register the connection to the router
     * @param conn connection
     */
    void Router::register_connection(connection_ptr conn) {
        boost::mutex::scoped_lock lk(m_connections_mutex);
        vector<connection_ptr>::iterator it;
        for (it = m_connections.begin(); it < m_connections.end(); ++it) {
            if (*it == conn) {
                // already registered, wtf?
                cout << "ERROR connection already registered!" << endl;
                assert(false);
                return;
            }
        }

//        std::cout << "register "<< conn->get("pubkey")<<endl;
        m_connections.push_back(conn);
//        cout << connections_str() << endl;
        // Add the conn to the router
        conn->set_name(conn->get("pubkey"));
    }

    void Router::unregister_connection(connection_ptr conn) {
        boost::mutex::scoped_lock lk(m_connections_mutex);
        vector<connection_ptr>::iterator it;
        for (it = m_connections.begin(); it < m_connections.end(); ++it) {
            if (*it == conn) {
                m_connections.erase(it);
                //cout << "Router::unregistered " << conn->str() << endl;
            }
        }
        //cout << connections_str() << endl;
    }

    connection_ptr
    Router::get_connection_by_name(const std::string &name) {
//        cout << "target name: " << name << endl;

        vector<connection_ptr>::iterator it;
        boost::mutex::scoped_lock lk(m_connections_mutex);
        for (it = m_connections.begin(); it < m_connections.end(); ++it) {
//            cout << "Name: " << (*it)->name() << endl;
            if ((*it)->name() == name) {
                return *it;
            }
        }
        std::cout << "Failed Find the connection" << std::endl;
        return connection_ptr();
    }

/// debug usage - get list of connections
    string
    Router::connections_str() {
        ostringstream os;
        os << "<connections>" << endl;
        BOOST_FOREACH(connection_ptr
                              conn, m_connections) {
                        os << conn->str() << endl;
                    }
        os << "</connections>" << endl;
        return os.str();
    }

    vector<string>
    Router::get_connected_names() {
        vector<string> v;
        boost::mutex::scoped_lock lk(m_connections_mutex);
        BOOST_FOREACH(connection_ptr
                              conn, m_connections) {
                        v.push_back(conn->name());
                    }
        return v;
    }

    /**
     * this is the default msg recvd callback passed to new connections
     * it does some basic sanity checks, then fires the callback
     * @param msgp
     * @param conn
     */
    void Router::message_received(message_ptr msgp, connection_ptr conn) {
//        cout << "router::message_received from " << conn->str()
//             << " " << msgp->str() << endl;

        if (msgp->length() > 16384) // hard limit
        {
            cout << "f2f router: Dropping, msg length: " << msgp->length() << endl;
            return;
        }
        m_protocol->message_received(msgp, conn);
    }

    void Router::send(std::string target, message_ptr msg) {
//        std::cout << "send 169" << std::endl;
//        std::cout << msg->str() << std::endl;
//
//        std::cout << "SEND " << target << std::endl;
        get_connection_by_name(target)->async_write(msg);
    }

    /**
     * Connect out to a remote Servent at endpoint
     * @param endpoint
     */
    void Router::connect_to_remote(boost::asio::ip::tcp::endpoint &endpoint) {
        map<string, string> props;
        connect_to_remote(endpoint, props);
    }

    void Router::connect_to_remote(boost::asio::ip::tcp::endpoint &endpoint, const map<string, string> &props) {
//        LOG("");
        cout << "router::connect_to_remote(" << endpoint.address().to_string() << ":"
             << endpoint.port() << ")" << endl;

        connection_ptr _conn = create_connection();
        typedef pair<string, string> pair_t;
        BOOST_FOREACH(pair_t
                              p, props) {
                        _conn->set(p.first, p.second);
                        cout << "p.first " << p.first << endl << " p.second " << p.second << endl;
                    }
        // Start an asynchronous connect operation.
        _conn->socket().async_connect(endpoint,
                                      boost::bind(&Router::handle_connect, this,
                                                  boost::asio::placeholders::error, endpoint, _conn));
    }

    /**
     * /// Handle completion of a connect operation.
     * @param e
     * @param endpoint
     * @param conn
     */
    void Router::handle_connect(const boost::system::error_code &e,
                                boost::asio::ip::tcp::endpoint &endpoint,
                                connection_ptr conn) {
        if (e) {
            std::cerr << "Failed to connect out to remote Servent: "
                      << e.message() << std::endl;
            return;
        }
        /// Successfully established connection.
        m_protocol->new_outgoing_connection(conn);
        register_connection(conn);
        conn->async_read(); // start read loop for this connection
    }

    /**
     * /// apply fun to all connections
     * @param fun
     */
    void Router::foreach_conns(boost::function<void(connection_ptr)> fun) {
        boost::mutex::scoped_lock lk(m_connections_mutex);
        BOOST_FOREACH(connection_ptr
                              conn, m_connections) {
                        fun(conn);
                    }
    }

    void Router::foreach_conns_except(boost::function<void(connection_ptr)> fun, connection_ptr conn) {
        boost::mutex::scoped_lock lk(m_connections_mutex);
        BOOST_FOREACH(connection_ptr
                              c, m_connections) {
                        if (c == conn)
                            continue;
                        fun(c);
                    }
    }

    /**
     * Broadcast
     * @param msgp
     */
    void Router::send_all(message_ptr msgp) {
        //foreach_conns( boost::bind(&Connection::async_write, _1, msgp) );
        boost::mutex::scoped_lock lk(m_connections_mutex);
        BOOST_FOREACH(connection_ptr
                              conn, m_connections) {
                        //cout << "Sending " << msgp->str() << " to " << conn->str() << endl;
                        conn->async_write(msgp);
                    }
    }

} // namespace libf2f
