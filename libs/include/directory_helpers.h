//
// Created by jin on 2/4/19.
//

#ifndef FILE_SENDER_DIRECTORY_HELPERS_H
#define FILE_SENDER_DIRECTORY_HELPERS_H

#include <string>
#include <vector>
#include <boost/filesystem.hpp>


namespace filesender {
    namespace helpers {

        struct path_leaf_string
        {
            std::string operator()(const boost::filesystem::directory_entry& entry) const
            {
                return entry.path().leaf().string();
            }
        };

        std::string make_path(const std::string &, const std::string &);
        bool is_file_exist(const std::string & file_name);
        std::vector<std::string> read_directory(const std::string &);
        bool check_type(const std::string &s, std::vector<std::string> &);
        std::string generate_dir_list(const std::string &, std::vector<std::string> &);
        bool directory_creator(const boost::filesystem::path &path);

    }
}


#endif //FILE_SENDER_DIRECTORY_HELPERS_H
