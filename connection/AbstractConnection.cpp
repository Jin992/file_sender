//
// Created by jin on 12/30/18.
//
#include "AbstractConnection.h"
#include <iostream>
#include "../log/Log.h"
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include "../files/FileHolder.h"
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/system/system_error.hpp>
#include "../exceptions/ConnectionException.h"
#include "../exceptions/FileException.h"
#include "../libs/include/directory_helpers.h"
#include <future>
#include <boost/asio/use_future.hpp>
#include <boost/asio/read.hpp>
#include <boost/optional.hpp>
#include <boost/bind.hpp>
#include <sys/epoll.h>

using filesender::exception::ConnectionException;
using filesender::exception::FileException;

namespace helper = filesender::helpers;

namespace filesender {
	namespace connection {

		AbstractConnection::AbstractConnection(io_service &io)
		: socket_(io), is_active_(true)
		 {}

		tcp::socket &AbstractConnection::socket() {
			return socket_;
		}

		void AbstractConnection::close_connection() {
			socket().close();
		}

		std::string AbstractConnection::read() {
			boost::asio::read(socket(), boost::asio::buffer(inbound_header_));
			// Checking incoming request for errors or end of stream
			std::istringstream is(std::string(inbound_header_, header_length));
			std::size_t inbound_data_size = 0;
			if (!(is >> std::hex >> inbound_data_size)) {
				throw ConnectionException("AbstractConnection::read : chunk size error.");
			}
			std::string result;
			// Resize bufer for exactly same size of sended data,
			// we need this step because read will be wait until it get full bufer
			inbound_data_.resize(inbound_data_size);
			// Read data
			boost::asio::read(socket(), boost::asio::buffer(inbound_data_));
			// Checking incoming request for errors or end of stream
			// Write received data to string
			std::string archive_data(&inbound_data_[0], inbound_data_.size());
			// Convert string to istringstream object for future conversion to
			// boost::archive::text_iarchive
			std::istringstream archive_stream(archive_data);
			boost::archive::text_iarchive archive(archive_stream);
			// Convert achive to string (deserialize)
			archive >> result;
			return result;
		}

		void AbstractConnection::send(const std::string &data) {

			// Serialize the data, to determine it's size.
			std::ostringstream archive_stream;
			boost::archive::text_oarchive archive(archive_stream);
			archive << data;
			outbound_data_ = archive_stream.str();

			// Format the header.
			std::ostringstream header_stream;
			header_stream << std::setw(header_length)
						  << std::hex << outbound_data_.size();
			if (!header_stream || header_stream.str().size() != header_length) {
				throw ConnectionException("AbstractConnection::send: invalid header length.");
			}
			outbound_header_ = header_stream.str();
			// Write serialized data to socket. We use "gather-write" to send
			// both the header and the data in a single write operation.
			std::vector<boost::asio::const_buffer> buffers;
			buffers.push_back(boost::asio::buffer(outbound_header_));
			buffers.push_back(boost::asio::buffer(outbound_data_));
			boost::asio::write(socket(), buffers);
		}

		void AbstractConnection::send_file_(const std::string &file_name, size_t chunk_size) {
			std::string local_buf;
			filesender::file::FileHolder file(chunk_size);
			file.open(file_name, "r");
			// read file chunk and send it.
			while ((file.read_chunk(local_buf, 1024)) > 0) {
				send(local_buf);
			}
			// indicate to reader end of file transfer stream
			send("<--stop");
			file.close();
		}

		void AbstractConnection::receive_file_(const std::string &file_name, size_t chunk_size) {
			filesender::file::FileHolder file(chunk_size);
			file.open(file_name, "w");
			std::string local_buf;
			// receive file chunk and write it to file.
			while ((local_buf = read()) != "<--stop") {
				if (local_buf.empty()) {
					throw ConnectionException("AbstractConnection::read: socket read error");
				}
				file.write_chunk(local_buf);
			}
			file.close();
		}

		void AbstractConnection::upload_file(const std::string &storage, const std::string &file) {
			if (!helper::is_file_exist(helper::make_path(storage, file))) {
				send("f_d_e"); // file doesn't exist
				throw FileException(" [" + file + "] file doesn't exist.");
			}
			send("OK"); // inform other side about file status
			send_file_(helper::make_path(storage, file), 1024);
		}

		void AbstractConnection::download_file(const std::string &storage, const std::string &file) {
			if (read() == "f_d_e") {
				throw FileException(" [" + file + "] file doesn't exist.");
			}
			receive_file_(helper::make_path(storage, file), 1024);
		}

		// return peer ip
		std::string AbstractConnection::get_remote() {
			return socket().remote_endpoint().address().to_string();
		}

		// return local ip
		std::string AbstractConnection::get_host() {
			return socket().local_endpoint().address().to_string();
		}

		// return local port
		unsigned short AbstractConnection::get_port() {
			return socket().local_endpoint().port();
		}

		// connection loop status
		bool AbstractConnection::get_connection_status() const {
			return is_active_;
		}

		// modify connection status
		void AbstractConnection::set_connection_status(bool const status) {
			is_active_ = status;
		}
	}
}