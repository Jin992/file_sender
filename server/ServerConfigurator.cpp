//
// Created by jin on 1/2/19.
//

#include "ServerConfigurator.h"
#include <iterator>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio.hpp>
#include "../log/Log.h"
#include <limits>
#include <iostream>
#include "../exceptions/ConfigException.h"
#include "../libs/include/directory_helpers.h"

using filesender::exception::ConfigException;
using filesender::log::Log;

namespace filesender {
	namespace config{

		namespace{
			std::vector<std::string> mimes = {".bmp", ".jpeg", ".jpg", ".png", ".gif"};
		}

		ServerConfigurator::ServerConfigurator(const std::string &path_to_config, const std::string &default_file_dir)
				: server_root_(default_file_dir), host_("127.0.0.1"), mime_types_(mimes), port_(8081), backlog_(0),
				  config_file_(path_to_config), connection_timeout_(30) {
			func_map_.insert(std::make_pair("host", &ServerConfigurator::verify_host_));
			func_map_.insert(std::make_pair("server_storage", &ServerConfigurator::verify_server_storage_));
			func_map_.insert(std::make_pair("port", &ServerConfigurator::verify_port_));
			func_map_.insert(std::make_pair("backlog", &ServerConfigurator::verify_backlog_));
			func_map_.insert(std::make_pair("timeout", &ServerConfigurator:: verify_connection_timeout_));
			func_map_.insert(std::make_pair("mime_types", &ServerConfigurator:: verify_mime_types_));
		}

		void ServerConfigurator::check_file_name() {
			std::vector<std::string> format;

			boost::algorithm::split(format, config_file_, boost::is_any_of("."));
			if (format.size() != 2 ) {
				throw ConfigException(std::string("incorect config file name should be server.config"));
			}
			if (*format.rbegin() != "config") {
				throw ConfigException(std::string("incorect type of config file ") + *format.rbegin());
			}
		}

		void ServerConfigurator::check_config_file() {
			if (!boost::filesystem::exists(config_file_)) {
				Log().warning(" config file doesn't exist.");
				Log().warning(" creating default [name].config file.");
				create_default_config_();
				Log().info("Server.config successfully created.");
			}
			Log().info("Reading config file.");
			read_config_file("./server.config");
			Log().info("Server configuration complete.");
		}

		std::string ServerConfigurator::get_host() const {
			return host_;
		}

		std::string ServerConfigurator::get_server_storage_dir() const {
			return server_root_;
		}

		int ServerConfigurator::get_port() const {
			return port_;
		}
		int ServerConfigurator::get_backlog() const {
			return backlog_;
		}
		std::vector<std::string> ServerConfigurator::get_mime_types() const {
			return mime_types_;
		}

		int ServerConfigurator::get_timeout() const {
			return connection_timeout_;
		}

		void ServerConfigurator::create_default_config_() const {
			std::ofstream os;

			os.open("./server.config");
			if (!os.is_open()) {
				throw ConfigException(std::string("ServerConfigurator::create_default_config_: can't create config file."));
			}
			os << "# File sender config file" << std::endl << std::endl;
			os << "# host address" << std::endl;
			os << "host " << host_ << std::endl << std::endl;
			os << "# port number" << std::endl;
			os << "port " << port_ << std::endl << std::endl;
			os << "# listen backlog size" << std::endl;
			os << "backlog " << backlog_ << std::endl << std::endl;
			os << "# client connection timeout in seconds" << std::endl;
			os << "timeout " << connection_timeout_ << std::endl << std::endl;
			os << "# mime types" << std::endl;
			os << "mime_types ";
			for (auto it = mime_types_.begin(); it != mime_types_.end(); it++) {
				os << *it ;
				if (it + 1 != mime_types_.end()) {
					os << ":";
				}
			}
			os << std::endl << std::endl;
			os << "# file storage folder" << std::endl;
			os << "server_storage" << " " << server_root_ << std::endl;
			os.close();
		}

		void ServerConfigurator::read_config_file(const std::string &path) {
			std::ifstream is(path);

			if (!is) {
				throw ConfigException(std::string("ServerConfigurator::read_config_file: cannot open config file ")
				+ path  + ".");
			}

			std::vector<std::string> raw_config;
			std::string str;
			while (std::getline(is, str)) {
				if (!str.empty()) {
					if ( *str.begin() != '#' && *str.begin() != ' ') {
						raw_config.push_back(str);
					}
				}
			}
			is.close();
			verify_config_file_(raw_config);
		}

		void ServerConfigurator::verify_config_file_(std::vector<std::string> raw_config) {
			if (raw_config.empty()) {
				throw ConfigException(std::string("ServerConfigurator::verify_config_file_: nothing to verify."));
			}
			for (auto it : raw_config) {
				std::vector<std::string> strs;

				boost::split(strs, it, boost::is_any_of(" "));
				if (strs.size() != 2) {
					throw ConfigException(std::string("ServerConfigurator::verify_config_file_: bad config file."));
				}

				std::map<std::string, pfunc>::const_iterator call;
				call = func_map_.find(*strs.begin());
				if (call == func_map_.end()) {
					throw ConfigException(
							std::string("ServerConfigurator::verify_config_file_: [") + *strs.begin() + "] bad param.");
				}
				(this->*(call->second))(strs);
			}
		}

		void ServerConfigurator::verify_host_(std::vector<std::string> &str) {
			boost::system::error_code err;
			boost::asio::ip::address::from_string(*str.rbegin(), err );
			if (err) {
				throw ConfigException(std::string("ServerConfigurator::verify_host_: ") + err.message());
			}
			host_ = *str.rbegin();
		}

		void ServerConfigurator::verify_server_storage_(std::vector<std::string> &str) {
			boost::filesystem::path path(*str.rbegin());
			if (!boost::filesystem::exists(path)) {
				filesender::helpers::directory_creator(path);
			}
			server_root_ = path.string();
		}

		void ServerConfigurator::verify_port_( std::vector<std::string> &str) {
			long long port = std::stoll(*str.rbegin(), nullptr, 10);
			if (port < 1023 || port > 65535) {
				throw ConfigException(std::string("ServerConfigurator::verify_port_: bad port number ."));
			}
			port_ = static_cast<int>(port);
		}

		void ServerConfigurator::verify_backlog_(std::vector<std::string> &str) {
			long long backlog = std::stoll(*str.rbegin(), nullptr, 10);

			if (backlog > std::numeric_limits<int>::max() ||
				backlog < std::numeric_limits<int>::min()) {
				throw ConfigException(std::string("ServerConfigurator::verify_backlog_: bad value of backlog [int limits]"));
			}
			if (backlog > 0 || static_cast<int>(backlog) > boost::asio::socket_base::max_connections) {
				throw ConfigException(std::string("ServerConfigurator::verify_backlog_: bad value of backlog"));
			}
			backlog_ = static_cast<int>(backlog);
		}

		void ServerConfigurator::verify_mime_types_(std::vector<std::string> &str) {
			std::vector<std::string> mimes;
			std::string out;

			boost::algorithm::split(mimes, *str.rbegin(), boost::is_any_of(":"));
			for (auto it : mimes) {
				out.append(it + " ");
			}
			mime_types_ = mimes;
		}

		void ServerConfigurator::verify_connection_timeout_(std::vector<std::string> &str) {
			long long timeout = std::stoll(*str.rbegin(), nullptr, 10);

			if (timeout > std::numeric_limits<int>::max() ||
				timeout < std::numeric_limits<int>::min()) {
				throw ConfigException(std::string("ServerConfigurator::verify_timeout_: bad value of timeout [int limits]"));
			}

			connection_timeout_ = static_cast<int>(timeout);
		}

	}
}