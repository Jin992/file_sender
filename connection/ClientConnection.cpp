//
// Created by jin on 1/30/19.
//
#include "ClientConnection.h"
#include "../files/FileHolder.h"
#include <boost/algorithm/string.hpp>
#include <boost/system/system_error.hpp>
#include "../exceptions/ConnectionException.h"
#include "../exceptions/FileException.h"
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include "../libs/include/directory_helpers.h"

using boost::asio::ip::tcp;
using boost::asio::io_service;
using filesender::connection::ClientConnection;
using filesender::log::Log;

namespace helper = filesender::helpers;

namespace filesender {
	namespace connection {
		ClientConnection::ClientConnection(io_service &io)
		: AbstractConnection(io), is_active_(true)
		{}

		ClientConnection::pointer ClientConnection::create(io_service &io) {
			return pointer(new ClientConnection(io));
		}

		std::string ClientConnection::get_remote() {
			return socket().remote_endpoint().address().to_string();
		}

		std::string ClientConnection::get_host()  {
			return socket().local_endpoint().address().to_string();
		}

		unsigned short ClientConnection::get_port() {
			return socket().local_endpoint().port();
		}

		void ClientConnection::terminate_connection() {
			if (socket().is_open()) {
				socket().close();
			}
		}
	}
}