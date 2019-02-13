//
// Created by jin on 1/4/19.
//
#include "FileHolder.h"
#include <boost/filesystem.hpp>
#include "../log/Log.h"
#include "../exceptions/FileException.h"

namespace filesender {
	namespace file {

		FileHolder::FileHolder(size_t chunk_size)
		: buf_(new char[chunk_size])
		{}

		FileHolder::~FileHolder() {
			if (file_.is_open()) {
				file_.close();
			}
			if (buf_ == nullptr) {
				delete [] buf_;
				buf_ = nullptr;
			}
		}

		// open file
		// mode = r - read
		// mode = w - write
		void FileHolder::open(const std::string &filename, const std::string &mode = "r") {
			file_name_ = filename;

			if (mode == "r"){
				file_.open(file_name_, std::fstream::in | std::fstream::binary);
			}
			else if (mode == "w") {
				file_.open(file_name_, std::fstream::out | std::fstream::trunc | std::fstream::binary);
			}
			else {
				throw filesender::exception::FileException("Fileholder object: bad mode [" + mode + "] choose r or w.");
			}

			if (!file_.is_open()) {
				throw filesender::exception::FileException("Fileholder object: can't open file");
			}
		}

		void FileHolder::close() {
			if (file_.is_open()) {
				file_.close();
			}
		}

		// read file by defined chunk-length
		ssize_t FileHolder::read_chunk(std::string &buf, size_t chunk_size) {
			if (!file_.is_open()) {
				throw filesender::exception::FileException("File read error");
			}
			if (file_.peek() == EOF) {
				return 0;
			}
			if  (!file_.read(buf_, chunk_size)) {
				if (file_.gcount() > 0) {
					return file_.gcount();
				}
				return  0;
			}
			buf = std::string(buf_, chunk_size);
			return file_.gcount();
		}

		// write file by defined chunk-length
		ssize_t FileHolder::write_chunk(std::string &chunk) {
			if (!file_.is_open()) {
				throw filesender::exception::FileException("Can't open file");
			}
			if  (!file_.write(&chunk[0] , chunk.size())) {
				return  0;
			}
			return 0;
		}
	}
}