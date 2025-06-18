
#include "Server.hpp"

Server	*globalServerPtr = NULL;

void signalHandler(int sig)
{
	if (sig == SIGINT)
		globalServerPtr->stop();
	std::cerr << std::endl;
}

int	main(int argc, char **argv)
{
	Server	server;
	globalServerPtr = &server;
	if (argc < 2)
	{
		std::cerr << "Usage: ./ircserv <port>" << std::endl;
		return (1);
	}
	// Setup signal handler
	std::signal(SIGINT, signalHandler);

	try
	{
		int	port = std::atoi(argv[1]);
		server.initServerConnection(port);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		server.closeServerConnection();
		return (1);
	}

	server.run();

	server.closeServerConnection();
	return (0);
}