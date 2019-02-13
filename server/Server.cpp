//
// Created by jin on 12/30/18.
//

#include "Server.h"
#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include "../connection/ServerConnection.h"
#include <boost/filesystem.hpp>
#include "../log/Log.h"
#include "../connection/ConnectionPool.h"
#include <thread>
#include "../exceptions/CustomException.h"

using filesender::connection::ServerConnection;
using filesender::connection::ConnectionPool;
using filesender::log::Log;

namespace filesender {
	Server::Server(boost::asio::io_service &io_service, const std::string &storage, int connection_timeout)
	: acceptor_(io_service), storage_(storage), server_status_(false), client_connection_timeout_(connection_timeout)
	{}

	// bind socket
	void Server::bind(const std::string &host, int port) {
		tcp::resolver resolver(acceptor_.get_io_service());
		tcp::resolver::query query(host, boost::lexical_cast<std::string>(port));
		tcp::endpoint endpoint = *resolver.resolve(query);
		acceptor_.open(endpoint.protocol());
		acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
		acceptor_.bind(endpoint);
	}

	// define listen socket
	void Server::listen(int backlog = 0) {
		if (backlog) {
			acceptor_.listen(backlog);
		} else {
			acceptor_.listen(boost::asio::socket_base::max_connections);
		}
	}

	void Server::run() {
		server_status_ = true;
		start_accept_();
	}

	void Server::set_mime_types(const std::vector<std::string> &mt) {
		mime_types_ = mt;
	}

	void Server::start_accept_() {
	    try {
			ConnectionPool pool;
			struct pollfd	sock_in[1];
			int poll_res = 0;

			sock_in[0].fd = acceptor_.native_handle();
			sock_in[0].events = POLLIN;
			// create tread pool to store connections
			pool.start();
			while (server_status_) {
				poll_res = poll(sock_in, 1, 300);
				if (poll_res > 0) {
					// create connection object
					ServerConnection::pointer new_connection = ServerConnection::create(acceptor_.get_io_service(),
							30);
					// assign incoming connection to connection object
					acceptor_.accept(new_connection->socket());
					Log().info("Accepted connection from " + new_connection->get_remote());
					// run connection in thread
					pool.store(new_connection, std::thread(boost::bind(&ServerConnection::start, new_connection,
							std::ref(storage_), std::ref(server_status_), std::ref(mime_types_))));
				}
				else if (poll_res == -1) {
					throw filesender::exception::CustomException("Server::accept: poll error");
				}
			}
			// server won't accept new connection
			// but all existing connections will be preccessed
			pool.stop();
		}
	    catch (boost::system::system_error &e) {
	    	Log().error(e.what());
	    }
	    catch (const std::exception &e) {
	        Log().error(e.what());
	    }
	}

    const tcp::acceptor & Server::get_acceptor() const {
	    return acceptor_;
	}
}