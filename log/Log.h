//
// Created by jin on 1/2/19.
//

#ifndef FILE_SENDER_LOG_H
#define FILE_SENDER_LOG_H

#include <string>
#include <mutex>

extern std::mutex g_stream_guard;

namespace filesender {
	namespace log {
		class Log {
		public:
			void info(const std::string &msg) ;
			void error(const std::string &msg) ;
			void warning(const std::string &msg) ;

		private:
			std::string get_current_time_() const;
		};

	}
}
#endif //FILE_SENDER_LOG_H
