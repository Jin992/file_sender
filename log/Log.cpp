//
// Created by jin on 1/2/19.
//

#include "Log.h"
#include <ctime>
#include <iostream>
#include <sstream>
#include <iomanip>

std::mutex g_stream_guard;

namespace filesender {
	namespace log {

		void Log::info(const std::string &msg){
			std::lock_guard<std::mutex> lock(g_stream_guard);

			std::clog << "\033[1;34m[log info]\033[0m["
					  << get_current_time_()
					  << "] " << msg << std::endl;
		}

		void Log::error(const std::string &msg){
			std::lock_guard<std::mutex> lock(g_stream_guard);

			std::clog << "\033[1;31m[log err ]\033[0m["
					  << get_current_time_()
					  << "] " << msg << std::endl;
		}

		void Log::warning(const std::string &msg) {
			std::lock_guard<std::mutex> lock(g_stream_guard);

			std::clog << "\033[1;33m[log warn]\033[0m["
					  << get_current_time_()
					  << "] " << msg << std::endl;
		}

		std::string Log::get_current_time_() const {
			time_t now = std::time(nullptr);
			tm *local_time = std::localtime(&now);

			std::stringstream ss;
			ss << std::put_time(local_time, "%d/%m/%Y %H:%M:%S");
			return ss.str();
		}
	}
}