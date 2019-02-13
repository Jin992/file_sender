//
// Created by jin on 1/31/19.
//

#ifndef FILE_SENDER_CONNECTIONPOOL_H
#define FILE_SENDER_CONNECTIONPOOL_H

#include "ServerConnection.h"
#include <mutex>
#include <vector>
#include <boost/thread.hpp>
#include <thread>
#include <utility>

using filesender::connection::ServerConnection;

namespace filesender {
    namespace connection{
        class ConnectionPool {
        public:
            explicit ConnectionPool();
            ConnectionPool(const ConnectionPool & rhs) = delete;
            ~ConnectionPool();
            void store(ServerConnection::pointer &, std::thread &&);
            size_t size() const;
            void connection_manager();
            void start();
            void stop();


        private:
            void erase_expired_conections_();
            bool is_active_;
            std::thread manager_thread_;
            std::vector<std::pair<ServerConnection::pointer, std::thread>> pool_storage_;
            std::mutex pool_guard_;

        };
    }
}



#endif //FILE_SENDER_CONNECTIONPOOL_H
