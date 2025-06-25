#include <iostream>
#include <csignal>
#include <cstdlib>				// atoi

#include "Server.hpp"

void signalHandler(int sig)
{
	if (sig == SIGINT)
		Server::getInstance().stop();
	std::cerr << std::endl;
}

int main(int argc, char** argv)
{
	if (argc != 3)
	{
		std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
		return 1;
	}
	// Setup signal handler
	std::signal(SIGINT, signalHandler);
	try 
	{
		Server::getInstance().definePassword(argv[2]);
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}
	
	int	port = std::atoi(argv[1]);
	try
	{
		Server::getInstance().initServerConnection(port);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		Server::getInstance().closeServerConnection();
		return 1;
	}

	Server::getInstance().run();
	Server::getInstance().closeServerConnection();
	return 0;
}