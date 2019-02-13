#include <iostream>
#include "../log/Log.h"
#include "../exceptions/ConfigException.h"
#include "../server/Server.h"
#include "../server/ServerConfigurator.h"

using filesender::log::Log;

int main()
{
	try {
		Log().info("File sender v1.0.");
        Log().info("Simple file server to store your favorite photos.");
		Log().info("Server initialization, please wait.");

		filesender::config::ServerConfigurator config("server.config", "./storage");
	    config.check_config_file();

		boost::asio::io_service io_service;
		filesender::Server server(io_service, config.get_server_storage_dir(), config.get_timeout());

        Log().info("Binding socket wait...");
		server.bind(config.get_host(), config.get_port());
        Log().info("Binding socket [success]");
        Log().info("Setting listen queue...");
		server.listen(config.get_backlog());
        Log().info("Setting listen queue [success]");
        Log().info("Starting main loop.");
        server.set_mime_types(config.get_mime_types());
        Log().info("Listening "
                            + std::to_string(server.get_acceptor().local_endpoint().port())
                            + " port on " + server.get_acceptor().local_endpoint().address().to_string() + ".");
        server.run();
	}
	catch (const filesender::exception::ConfigException &e) {
		Log().error(e.what());
	}
	catch (const std::exception& e) {
		Log().error(e.what());
	}
	return 0;
}
