//
// Created by jin on 1/30/19.
//

#include "CustomException.h"
namespace filesender {
	namespace exception {

		CustomException::CustomException(const std::string &msg) : msg_(msg)
		{}

		char const *CustomException::what() const noexcept {
			return msg_.c_str();
		}
	}
}