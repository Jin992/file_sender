//
// Created by jin on 1/30/19.
//

#include "ConfigException.h"

namespace filesender {
    namespace exception {
        ConfigException::ConfigException(const std::string &msg)
                : CustomException(msg)
        {}
    }
}