//
// Created by jin on 2/4/19.
//

#include "../include/directory_helpers.h"

#include <algorithm>
#include <boost/algorithm/string.hpp>

using boost::filesystem::directory_iterator;
using boost::filesystem::path;

namespace filesender {
    namespace helpers {
        // create path string
        std::string make_path(const std::string &storage, const std::string &file) {
            return std::string(storage + "/" + file);
        }

        // check file existance
        bool is_file_exist(const std::string & file_name) {
            if (boost::filesystem::exists(file_name)) {
                return true;
            }
            return false;
        }

        // get directory contents
        std::vector<std::string> read_directory(const std::string &dir_name) {
            std::vector<std::string> dir;
            path p(dir_name);
            directory_iterator start(p);
            directory_iterator end;
            std::transform(start, end, std::back_inserter(dir), path_leaf_string());
            return dir;
        }

        // check mime types
        bool check_type(const std::string &s, std::vector<std::string> &mimes) {
            std::vector<std::string> file;
            boost::algorithm::split(file, s, boost::is_any_of("."));
            if (std::find(mimes.begin(), mimes.end(), *file.rbegin()) != mimes.end()) {
                return true;
            }
            return false;
        }

        // generate one string from list
        std::string generate_dir_list(const std::string &dir_name, std::vector<std::string> &mimes) {
            std::vector<std::string> dir = read_directory(dir_name);
            std::vector<std::string> valid_files;
            std::copy_if(dir.begin(), dir.end(), std::back_inserter(valid_files), std::bind(&check_type, std::placeholders::_1, mimes));
            std::string file_list;
            for (auto it: valid_files) {
                file_list.append(it + "+");
            }
            return file_list;
        }

        // recursively create folders
        bool directory_creator(const boost::filesystem::path &path) {
            if (!boost::filesystem::exists(path)) {
                directory_creator(path.parent_path());
            }
            boost::filesystem::create_directory(path);
            return true;
        }
    }
}