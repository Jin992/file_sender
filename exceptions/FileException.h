//
// Created by jin on 1/30/19.
//

#ifndef FILE_SENDER_FILEEXCEPTION_H
#define FILE_SENDER_FILEEXCEPTION_H

#include "CustomException.h"

namespace filesender {
	namespace exception {
		class FileException : public CustomException {
		public:
			explicit FileException(const std::string &msg);
		};
	}
}


#endif //FILE_SENDER_FILEEXCEPTION_H
