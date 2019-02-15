//
// Created by jin on 1/4/19.
//

#ifndef FILE_SENDER_FILEHOLDER_H
#define FILE_SENDER_FILEHOLDER_H

#include <string>
#include <fstream>

namespace filesender {
	namespace file {
		using file_error = int;

		class FileHolder {
		public:
			explicit FileHolder(size_t chunk_size);
			FileHolder(FileHolder const &rhs) = delete;
			virtual ~FileHolder();

			FileHolder &operator=(FileHolder const  &rhs) = delete;

			void open(const std::string &, const std::string &);
			void close();
			ssize_t read_chunk(std::string &buf, size_t chunk_size);
			ssize_t write_chunk(std::string &chunk);

		private:
			std::string file_name_;
			char *buf_;
			std::fstream file_;
		};
	}
}

#endif //FILE_SENDER_FILEHOLDER_H
