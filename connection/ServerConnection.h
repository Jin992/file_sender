//
// Created by jin on 1/30/19.
//

#ifndef FILE_SENDER_SERVERCONNECTION_H
#define FILE_SENDER_SERVERCONNECTION_H

#include "AbstractConnection.h"
#include <thread>

using timepoint_t = std::chrono::time_point<std::chrono::high_resolution_clock>;

namespace filesender {
	namespace connection {
		class ServerConnection : public AbstractConnection {
		public:
			typedef boost::shared_ptr<ServerConnection> pointer;

			ServerConnection(const ServerConnection & rhs) = delete;
			~ServerConnection() override = default;

			static pointer create(io_service &, int);
			void start(const std::string &, bool &, std::vector<std::string> &);
			void terminate_connection();
			long get_timeout();

		private:
			explicit ServerConnection(io_service &, int);
			void set_l_activity_(time_point_t &&);
			time_point_t get_l_activity_();



		private:
			time_point_t last_activity_;
			long timeout_;


		};
	}
}



#endif //FILE_SENDER_SERVERCONNECTION_H
