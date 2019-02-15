//
// Created by jin on 1/30/19.
//
#include "ServerConnection.h"
#include "../files/FileHolder.h"
#include <boost/algorithm/string.hpp>
#include <boost/system/system_error.hpp>
#include "../exceptions/ConnectionException.h"
#include "../exceptions/FileException.h"
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include "../libs/include/directory_helpers.h"
#include "../exceptions/ConnectionException.h"

using boost::asio::ip::tcp;
using boost::asio::io_service;
using filesender::connection::ServerConnection;
using filesender::log::Log;
using filesender::exception::ConnectionException;

namespace helper = filesender::helpers;


namespace filesender {
	namespace connection {
		ServerConnection::ServerConnection(io_service &io, int timeout)
		: AbstractConnection(io), last_activity_(std::chrono::high_resolution_clock::now()),
		timeout_(timeout * 1000)
		{}

		ServerConnection::pointer ServerConnection::create(io_service &io, int timeout) {
			return pointer(new ServerConnection(io, timeout));
		}

		// stoping loop and close socket
		void ServerConnection::terminate_connection() {
			set_connection_status(false);
			if (socket().is_open()) {
				socket().close();
			}
			Log().info("Connection with " + get_remote() + "has been closed");
		}

		// set last connection activity time
		void ServerConnection::set_l_activity_(time_point_t &&last_activity) {
			last_activity_ = last_activity;
		}

		// get last connection activity time
		time_point_t ServerConnection::get_l_activity_() {
			return last_activity_;
		}

		long ServerConnection::get_timeout() {
			return timeout_;
		}

		// connection main loop
		void ServerConnection::start(const std::string &storage_dir, bool &server_status,  std::vector<std::string> &mimes) {
			Log().info(get_host() + "->" + get_remote() + " connection established");
			struct pollfd	sock_in[1];
			int poll_res = 0;

			sock_in[0].fd = socket().native_handle();
			sock_in[0].events = POLLIN;

			while (get_connection_status()) {
				try {
					std::vector<std::string> result;

					// determine the time what connection stay without activity
					// if it bigger than timer close connection
					long milli = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - get_l_activity_()).count();
					if (milli  > get_timeout()) {
					   break;
					}

					// polling socket
					// if we have something to read, read it
					// else skip to new loop iteratiion
					poll_res = poll(sock_in, 1, 300);
					if (poll_res > 0) {
						std::string recv = read();

						boost::algorithm::split(result, recv, boost::is_any_of(" "));
						if (result.empty()) {
							Log().info("empty request.");
							continue;
						}
						if (*result.begin() == "download" && result.size() == 2) {
							set_l_activity_(std::chrono::high_resolution_clock::now());
							Log().info(get_remote() + " download request [" + helper::make_path(storage_dir, *result.rbegin()) + "]");
							upload_file(storage_dir, *result.rbegin());
							Log().info(helper::make_path(storage_dir, *result.rbegin()) + " has been uploaded to " + get_remote());
						} else if (*result.begin() == "upload" && result.size() == 2) {
							set_l_activity_(std::chrono::high_resolution_clock::now());
							Log().info(get_remote()  + " upload request [" + helper::make_path(storage_dir, *result.rbegin()) + "]");
							download_file(storage_dir, *result.rbegin());
							Log().info(helper::make_path(storage_dir, *result.rbegin()) + " has been downloaded from " + get_remote());
						} else if (*result.begin() == "ls" && result.size() == 1) {
							set_l_activity_(std::chrono::high_resolution_clock::now());
							Log().info(get_remote() + " requested directory list");
							std::string file_list = helper::generate_dir_list(storage_dir, mimes);
							send(file_list);
							Log().info("directory list has been sended to " + get_remote());
						} else if (*result.begin() == "disconnect" && result.size() == 1) {
							send("disconnected");
							terminate_connection();
						} else if (*result.begin() == "shutdown" && result.size() == 1) {
							Log().info("Received server shutdown request.");
							server_status = false;
							Log().info("Server Terminated.");
							terminate_connection();
						} else {
							set_l_activity_(std::chrono::high_resolution_clock::now());
						}
					}
					else if (poll_res == -1) {
						throw ConnectionException("ServerConnection::send: poll error");
					}
				}
				catch (const filesender::exception::ConnectionException &e) {
					Log().error(e.what());
					terminate_connection();
				}
				catch (const filesender::exception::FileException &e) {
					Log().info(get_remote() + std::string(" ") + std::string(e.what()));
				}
				catch (const boost::system::system_error &ex) {
					Log().info("Connection closed");
					break;
				}
				catch (const std::exception &e) {
					Log().error(e.what());
					Log().error("Critical error, connection terminated.");
					break;
				}
			}
			close_connection();
		}
	}
}