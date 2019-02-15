//
// Created by jin on 1/5/19.
//
#include <boost/algorithm/string.hpp>
#include <vector>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include "../libs/include/directory_helpers.h"
#include "../log/Log.h"
#include "../connection/ClientConnection.h"
#include "../exceptions/FileException.h"
#include "../exceptions/ConnectionException.h"

using boost::asio::ip::tcp;
using filesender::connection::ClientConnection;

namespace {
	void parse_dir(const std::string dir) {
		std::vector<std::string> file_list;
		auto last_elem = std::next(file_list.end(), -1);;

		boost::algorithm::split(file_list, dir, boost::is_any_of("+"));
		for (auto it = file_list.begin(); it != file_list.end(); ++it) {
			std::cout << *it;
			if (it != last_elem) {
				std::cout << std::endl;
			}
		}
		file_list.clear();
	}

	void check_dir(const std::string & str) {
		boost::filesystem::path path(str);
		if (!boost::filesystem::exists(path)) {
			filesender::helpers::directory_creator(path);
		}
	}

	void list_client_storage(const std::string &storage) {
		std::vector<std::string> dir_list = filesender::helpers::read_directory(storage);
		for (auto it: dir_list) {
			std::cout << it << std::endl;
		}
		std::cout << std::endl;
	}


	unsigned short validate_port(char *port) {
		long long tmp_port = std::stoll(port, nullptr, 10);
		if (tmp_port < 1023 || tmp_port > 65535) {
			throw std::invalid_argument("Bad port number: set port in range 1023 - 65535.");
		}
		return static_cast<unsigned short>(tmp_port);
	}

}


// argv[1] - ip address
// argv[2] - port
// argv[3] - file storage directory
int main(int argc, char **argv) {

	if (argc != 4) {
		std::cout << "usage:" << std::endl;
		std::cout << argv[0] << " [server ip] [server port] [storage dir]" << std::endl;
		return 1;
	}
	// creating connection object
	boost::asio::io_service io_service;
	ClientConnection::pointer con = ClientConnection::create(io_service);
	try {
		// validating given address and port
		boost::asio::ip::address ip = boost::asio::ip::address::from_string(argv[1]);
		unsigned short port = validate_port(argv[2]);

		// creating storage directory if not exist
		check_dir(argv[3]);

		// Establish connection with server
		con->socket().connect(tcp::endpoint(ip, port));
	}
	catch (const boost::system::system_error &ex) {
		std::cout << "Connection refused" << std::endl;
		return 1;
	}
	catch (const std::exception &e) {
		std::cout << e.what() << std::endl;
		return 1;
	}

	std::cout << "Filesender simple file storage for your favorite photos" << std::endl;
	std::cout << "Connected to " << con->get_remote() << std::endl;
	// main loop
	while (true) {
		try {
			std::string line;
			std::vector<std::string> operation;
		   if (!con->socket().is_open()) {
		break;
		}
					
			std::cout << "fs-client > ";
			std::getline(std::cin, line);
			boost::algorithm::split(operation, line, boost::is_any_of(" "));
			if (operation.empty()) {
				continue;
			}
			if (*operation.begin() == "download" && operation.size() == 2) {
			std::cout << *operation.rbegin() << " download started." << std::endl;    
			con->send(line);
			con->download_file(argv[3], *operation.rbegin());
			std::cout << *operation.rbegin() << " download complete." << std::endl;
			}
			else if (*operation.begin() == "upload" && operation.size() == 2) {
			std::cout << *operation.rbegin() << " upload started." << std::endl;
			con->send(line);
			con->upload_file(argv[3], *operation.rbegin());
			std::cout << *operation.rbegin() << " upload complete." << std::endl; 
			}
			else if (*operation.begin() == "ls" && operation.size() == 1) {
				list_client_storage(argv[3]);
			}
			else if (*operation.begin() == "ls" && operation.size() == 2) {
				if (*operation.rbegin() == "server") {
					con->send(*operation.begin()) ;
					std::string files = con->read();
					parse_dir(files);
				}
				else {
					std::cout << "tip:" << std::endl;
					std::cout << "\tls - to list local file storage." << std::endl;
					std::cout << "\tls server - to list server file storage." << std::endl;
				}
			}
			else if (*operation.begin() == "quit" && operation.size() == 1) {
				break;
			}
			else if (*operation.begin() == "shutdown" && operation.size() == 1) {
				con->send("shutdown");
				break;
			}
			else if (*operation.begin() == "help") {
				std::cout << "Help:"<< std::endl;
				std::cout << "\tdownload [file]   - download [file] from server." << std::endl;
				std::cout << "\tupload [file]     - upload [file] to server." << std::endl;
				std::cout << "\tls                - get local storage file list." << std::endl;
				std::cout << "\tls server         - get local storage file list." << std::endl;
				std::cout << "\tquit              - disconnect from server and quit client" << std::endl;
				std::cout << "\tshutdown          - shutdown server" << std::endl;
			}
			else {
				std::cout << "bad command use 'help'" << std::endl;
			}
		}
		catch (const filesender::exception::ConnectionException &e) {
			std::cout << e.what() << std::endl;
			break;
		}
		catch (const filesender::exception::FileException &e) {
			std::cout << con->get_remote() << " " << e.what() << std::endl;
		}
		catch (const boost::system::system_error &ex) {
			std::cout << "Connection closed by server" << std::endl;
			break;
		}
		catch (const std::exception &e) {
			std::cout << e.what() << std::endl;
			std::cout << "Critical error, connection terminated." << std::endl;
			break;
		}
	}
	return 0;
}
