//
// Created by jin on 1/2/19.
//

#ifndef FILE_SENDER_SERVERCONFIGURATOR_H
#define FILE_SENDER_SERVERCONFIGURATOR_H

#include <string>
#include <vector>
#include <map>

namespace filesender {
	namespace config {
		class ServerConfigurator {
		public:
			typedef void (ServerConfigurator::*pfunc)(std::vector<std::string> &);

			ServerConfigurator(const std::string &path_to_config, const std::string &default_file_dir);
			void check_file_name();
			void check_config_file();
			std::string get_host() const;
			std::string get_server_storage_dir() const;
			int get_port() const;
			int get_backlog() const;
			int get_timeout() const;
			std::vector<std::string> get_mime_types() const;

		private:
			void create_default_config_() const;
			void read_config_file(const std::string &path);
			void verify_config_file_(std::vector<std::string> raw_config);
			void verify_host_(std::vector<std::string> &str);
			void verify_server_storage_(std::vector<std::string> &str);
			void verify_port_( std::vector<std::string> &str);
			void verify_backlog_(std::vector<std::string> &str);
			void verify_mime_types_(std::vector<std::string> &str);
			void verify_connection_timeout_(std::vector<std::string> &str);

		private:
			std::string						server_root_;
			std::string						host_;
			std::vector<std::string>		mime_types_;
			int								port_;
			int								backlog_;
			std::map<std::string, pfunc>	func_map_;
			std::string						config_file_;
			int						connection_timeout_;

		};

	}
}


#endif //FILE_SENDER_SERVERCONFIGURATOR_H
