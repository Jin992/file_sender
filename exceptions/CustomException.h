//
// Created by jin on 1/30/19.
//

#ifndef FILE_SENDER_CUSTOMEXCEPTIONS_H
#define FILE_SENDER_CUSTOMEXCEPTIONS_H

#include <exception>
#include <string>

namespace filesender {
    namespace exception {

        class CustomException : public std::exception {
        public:
            explicit CustomException(const std::string &msg);

            char const *what() const noexcept final;

        private:
            std::string msg_;
        };
    }
}

#endif //FILE_SENDER_CUSTOMEXCEPTIONS_H
