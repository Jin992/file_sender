//
// Created by jin on 1/30/19.
//

#include "FileException.h"

namespace filesender {
	namespace exception {
		FileException::FileException(const std::string &msg)
		: CustomException(msg)
		{}
	}
}