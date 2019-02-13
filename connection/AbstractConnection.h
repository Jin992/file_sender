//
// Created by jin on 12/30/18.
//

#ifndef FILE_SENDER_CONNECTION_H
#define FILE_SENDER_CONNECTION_H

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <cstring>
#include <chrono>
#include "../files/FileHolder.h"
#include "../log/Log.h"

namespace filesender {
    namespace connection {

        using boost::asio::ip::tcp;
        using boost::asio::io_service;
        using time_point_t = std::chrono::time_point<std::chrono::system_clock>;

        class AbstractConnection : public boost::enable_shared_from_this<AbstractConnection> {
        public:
           explicit AbstractConnection(io_service &);
           tcp::socket &socket();
           void close_connection();
           std::string read();
           void send(const std::string &data);
           void upload_file(const std::string &storage, const std::string &file);
           void download_file(const std::string &storage, const std::string &file);
           std::string get_remote();
           std::string get_host();
           unsigned short get_port();
           bool get_connection_status() const;
           void set_connection_status(bool const status);
           virtual ~AbstractConnection() = 0;

        private:
            void send_file_(const std::string &file, size_t chunk_size);
            void receive_file_(const std::string &buf, size_t chunk_size);

        private:
            tcp::socket socket_;
            bool is_active_;
            enum {
                header_length = 8
            };
            std::string outbound_header_;
            std::string outbound_data_;
            char inbound_header_[header_length];
            std::vector<char> inbound_data_;
        };

        inline AbstractConnection::~AbstractConnection() = default;
    }
}
#endif //FILE_SENDER_CONNECTION_H
