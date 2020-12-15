#ifndef __LIBF2F_MESSAGE_H__
#define __LIBF2F_MESSAGE_H__

#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include <iostream>

namespace libf2f {

    class Message;

    typedef boost::shared_ptr<Message> message_ptr;

    class Connection;

    class Router;

    typedef boost::shared_ptr<Connection> connection_ptr;
    typedef boost::weak_ptr<Connection> connection_ptr_weak;

/*
        Bytes   Description
        -------------------
        0      Msg Type (see Message::Type enum)
        1-4    Payload Length
        5-     Payload

*/

/// All messages start with this header:
    struct message_header {
        char type;
        boost::uint32_t length;
    };

    class Message {
    public:

        Message()
                : m_payload(0) {}

        Message(const message_header &header)
                : m_payload(0) {
            m_header = header;
            //std::cout << "CTOR Msg(" << m_guid << ")" << std::endl;
        }

        virtual ~Message() {
            //std::cout << "DTOR Msg(" << m_guid << ")" << std::endl;
            if (m_payload) free(m_payload);
        }

        virtual const boost::uint32_t total_length() const {
            return sizeof(message_header) + length();
        }

        virtual const std::string str() const {
            std::ostringstream os;
            os << "[Msg type:" << (int) type()
               << " length:" << (int) length() << "]";
            return os.str();
        }

        virtual message_header &header() { return m_header; }

        virtual const char type() const { return m_header.type; }

        virtual const boost::uint32_t length() const { return ntohl(m_header.length); }

        // payload
        virtual const char *payload() const { return m_payload; }

        virtual char *payload() { return m_payload; }

        /// sucks to have to do this really, jsonspirit needs a string or stream:
        virtual std::string payload_str() const {
            std::string s(m_payload, length());
            return s;
        }

        virtual size_t malloc_payload() {
            if (length() == 0) return 0;
            free(m_payload);
            m_payload = (char *) malloc(length());
            return length();
        }

        virtual const boost::asio::mutable_buffer payload_buffer() const {
            return boost::asio::buffer(m_payload, length());
        }

        virtual std::vector<boost::asio::const_buffer> to_buffers() const {
            std::vector<boost::asio::const_buffer> buffers;
            buffers.push_back(boost::asio::buffer(
                    (char *) &m_header, sizeof(message_header)));
            if (length()) {
                buffers.push_back(boost::asio::buffer(m_payload, length()));
            }

            return buffers;
        }

    protected:
        message_header m_header;
        mutable std::string m_guid;
        char *m_payload;
    };

    class GeneralMessage : public Message {
    public:
        GeneralMessage(const char msgtype, const std::string &body) {
            message_header h;
//            memcpy(&h.guid, uuid.data(), 36);
            h.type = msgtype;
            h.length = htonl(body.length());
            m_header = h;
            malloc_payload();
            memcpy(m_payload, body.data(), body.length());

//            std::cout <<std::endl<<ntohl( h.length)<<std::endl;
        }
    };


}
#endif
