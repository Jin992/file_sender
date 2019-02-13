//
// Created by jin on 1/30/19.
//

#ifndef FILE_SENDER_CONFIGEXCEPTION_H
#define FILE_SENDER_CONFIGEXCEPTION_H

#include "CustomException.h"

namespace filesender {
    namespace exception {
        class ConfigException : public CustomException {
        public:
            explicit ConfigException(const std::string &msg);
        };
    }
}

#endif //FILE_SENDER_CONFIGEXCEPTION_H
