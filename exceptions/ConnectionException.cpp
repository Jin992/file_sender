//
// Created by jin on 1/30/19.
//

#include "ConnectionException.h"

namespace filesender {
	namespace exception {
		ConnectionException::ConnectionException(const std::string  &msg) : CustomException(msg)
		{}
	}
}