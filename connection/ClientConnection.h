//
// Created by jin on 1/30/19.
//

#ifndef FILE_SENDER_CLIENTCONNECTION_H
#define FILE_SENDER_CLIENTCONNECTION_H

#include "AbstractConnection.h"
#include <thread>


namespace filesender {
	namespace connection {
		class ClientConnection : public AbstractConnection {
		public:
			typedef boost::shared_ptr<ClientConnection> pointer;

			ClientConnection(const ClientConnection & rhs) = delete;
			~ClientConnection() override = default;
			static pointer create(io_service &);
		
		private:
			explicit ClientConnection(io_service &);
			bool is_active_;
		};
	}
}



#endif //FILE_SENDER_ClientConnection_H
