//
// Created by jin on 1/31/19.
//

#include "ConnectionPool.h"
#include <chrono>
#include <algorithm>
#include "../log/Log.h"



namespace filesender {
    namespace connection {
        using std::chrono::system_clock;

        ConnectionPool::ConnectionPool()
        : is_active_(true), manager_thread_()
        {}

        ConnectionPool::~ConnectionPool() {
            if (manager_thread_.joinable()) {
                manager_thread_.join();
            }
        }

        void ConnectionPool::store(ServerConnection::pointer &con_ptr, std::thread &&thd) {
            std::lock_guard<std::mutex> lock(pool_guard_);
            pool_storage_.push_back(std::make_pair(con_ptr, std::move(thd)));
        }

        size_t ConnectionPool::size() const {
            return pool_storage_.size();
        }

        void ConnectionPool::start() {
            manager_thread_ = std::thread(&ConnectionPool::connection_manager, this);
        }

        void ConnectionPool::stop() {
            is_active_ = false;
            while (!pool_storage_.empty()) {
                erase_expired_conections_();
                ::usleep(1000000);
            }
        }

        // join thread and erase non-active connections from pool
        void ConnectionPool::erase_expired_conections_() {
            std::lock_guard<std::mutex> lock(pool_guard_);

            auto to_join = std::remove_if(pool_storage_.begin(), pool_storage_.end(), [](std::pair<ServerConnection::pointer , std::thread> &con) {
                if (con.first->socket().is_open()) {
                    return false;
                }
                if (con.second.joinable()) {
                        con.second.join();
                        return true;
                    }
                return false;
            });
            pool_storage_.erase(to_join, pool_storage_.end());
        }

        // thread worker that check connection activity
        void ConnectionPool::connection_manager() {
            while (is_active_) {
                erase_expired_conections_();
                ::usleep(1000000);
            }
        }
    }
}