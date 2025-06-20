#include "Server.hpp"

#include <iostream>
#include <unistd.h>
#include <cerrno>
#include <cstring>				// memset
#include <fcntl.h>
#include <netdb.h>				// gethostbyname, getprotobyname, htons, htonl
#include <sys/socket.h>			// socket, bind, accept, listen

#include "UserManager.hpp"
#include "IrcException.hpp"

Server::Server()
{
	running_ = false;
	// Initialize pollfd for listening socket
	pollfd_.fd = -1;
	pollfd_.events = POLLIN;
	pollfd_.revents = 0;
}

void Server::closeServerConnection()
{
	UserManager::getInstance().closeAllConnection();
	if (pollfd_.fd != -1)
		close (pollfd_.fd);
	std::cout << "Shutting down server..." << std::endl;
}

void Server::initServerConnection(int port)
{
	// Get protocol number for TCP
	struct protoent *proto = getprotobyname("tcp");
	if (!proto)
		throw std::runtime_error("Error: getprotobyname failed");
	// Creating the Server Socket
	pollfd_.fd = socket(AF_INET, SOCK_STREAM, 0);
	if (pollfd_.fd < 0)
		throw std::runtime_error("Error: cannot open socket");
	// Set SO_REUSEADDR to avoid "address already in use" error on restart
	int opt = 1;
	if (setsockopt(pollfd_.fd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) < 0)
		throw std::runtime_error("Error: setsockopt failed");
	// Bind to all interfaces
	struct sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(pollfd_.fd, (struct sockaddr *) &addr, sizeof(addr)) < 0)
		throw std::runtime_error("Error: bind failed");
	// Listen
	if (listen(pollfd_.fd, SOMAXCONN) < 0) 
		throw std::runtime_error("Error: listen failed");
	std::cout << "Server listening on port " << port << " ..." << std::endl;
}

void Server::run()
{
	running_ = true;
	while (running_)
	{
		int poll_count = UserManager::getInstance().getNbUserEvent(); // 5 sec timeout
		if (poll_count == 0)
			continue; // Timeout, no events, loop again
		if (poll_count < 0)
		{
			if (errno == EINTR)
				continue; // Interrupted by signal, just continue
			std::cerr << "Error: poll" << std::endl;
			break;
		}
		try
		{
			checkNewConnections(); // Check if listening socket is readable => new connection
		}
		catch(const IrcException& e)
		{
			std::cerr << e.what() << std::endl;
			continue;
		}
		UserManager::getInstance().listenClients();
	}
}

void Server::stop()
{
	running_ = false;
}

void Server::checkNewConnections()
{
	if (pollfd_.revents & POLLIN)
	{
		struct sockaddr_in clientAddr;
		socklen_t clientLen = sizeof(clientAddr);
		int clientSocket = accept(pollfd_.fd, (struct sockaddr*)&clientAddr, &clientLen);
		if(fcntl(clientSocket, F_SETFL, O_NONBLOCK) < 0)
			throw std::runtime_error("Error: fcntl failed, cannot connect client");
		if (clientSocket < 0)
			throw std::runtime_error("Error: accept failed");
		else
			addNewUser(clientSocket, clientAddr);
	}
}

void Server::addNewUser(int clientSocket, struct sockaddr_in clientAddr)
{
	UserManager::getInstance().addNewUser(clientSocket, clientAddr);
	std::cout << "Accepted connection from "
		<< inet_ntoa(clientAddr.sin_addr) << ":"
		<< ntohs(clientAddr.sin_port) << std::endl;
}