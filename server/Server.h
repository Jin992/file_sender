//
// Created by jin on 12/30/18.
//

#ifndef FILE_SENDER_SERVER_H
#define FILE_SENDER_SERVER_H

#include <boost/asio.hpp>
#include <string>
#include "ServerConfigurator.h"

using boost::asio::ip::tcp;


namespace filesender{
	class Server {
	public:
		// Initizlize accept object
		explicit Server(boost::asio::io_service &, const std::string &, int);
		// Binding socket to host and port
		void bind(const std::string &, int);
		// Starting to listing socket for incomming connections from peers
		// int backlog - is size of the incomming connection queue
		// if zero system apply max queue size
		void listen(int);
		void set_mime_types(const std::vector<std::string> &);

		// Starting main loop
		void run();
		const tcp::acceptor &get_acceptor() const;

	private:
		void start_accept_();

	private:
		tcp::acceptor acceptor_;
		std::string storage_;
		bool server_status_;
		std::vector<std::string> mime_types_;
		int client_connection_timeout_;
	};
}
#endif //FILE_SENDER_SERVER_H
