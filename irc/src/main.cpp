#include <iostream>
#include <csignal>

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
	std::signal(SIGINT, signalHandler); // Setup signal handler
	int status = Server::getInstance().up(argv[1], argv[2]);
	return status;
}