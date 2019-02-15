//
// Created by jin on 1/30/19.
//

#ifndef FILE_SENDER_CONNECTIONEXCEPTION_H
#define FILE_SENDER_CONNECTIONEXCEPTION_H

#include "CustomException.h"

namespace filesender {
	namespace exception {
		class ConnectionException : public CustomException {
		public:
			explicit ConnectionException(const std::string &msg);
		};

	}

}


#endif //FILE_SENDER_CONNECTIONEXCEPTION_H
