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
	serverSocket_ = -1;
}

void Server::closeServerConnection()
{
	UserManager::getInstance().closeAllConnection();
	if (serverSocket_ != -1)
		close (serverSocket_);
	std::cout << "Shutting down server..." << std::endl;
}

void Server::initServerConnection(int port)
{
	// Get protocol number for TCP
	struct protoent *proto = getprotobyname("tcp");
	if (!proto)
		throw std::runtime_error("Error: getprotobyname failed");
	// Creating the Server Socket
	serverSocket_ = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket_ < 0)
		throw std::runtime_error("Error: cannot open socket");
	// Set SO_REUSEADDR to avoid "address already in use" error on restart
	int opt = 1;
	if (setsockopt(serverSocket_, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) < 0)
		throw std::runtime_error("Error: setsockopt failed");
	// Bind to all interfaces
	std::memset(&serverAddr_, 0, sizeof(serverAddr_));
	serverAddr_.sin_family = AF_INET;
	serverAddr_.sin_port = htons(port);
	serverAddr_.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(serverSocket_, (struct sockaddr *) &serverAddr_, sizeof(serverAddr_)) < 0)
		throw std::runtime_error("Error: bind failed");
	// Listen
	if (listen(serverSocket_, SOMAXCONN) < 0) 
		throw std::runtime_error("Error: listen failed");
	UserManager::getInstance().initUserManager(serverSocket_, serverAddr_);
	std::cout << "Server listening on port " << port << " ..." << std::endl;
}

void Server::initPassword(std::string password)
{
	if (password.empty())
		throw std::runtime_error("Error: Password is empty");
	for (size_t i = 0; i < password.length(); ++i)
	{
        unsigned char c = password[i];
        if (c <= 32 || c > 126) {
            throw std::runtime_error("Error: Use just ascii value 33 at 126");
        }
    }
	password_ =  password;
}

const std::string & Server::getPassword()
{
	return password_;
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
	if (UserManager::getInstance().serverReiceveData())
	{
		struct sockaddr_in clientAddr;
		socklen_t clientLen = sizeof(clientAddr);
		int clientSocket = accept(serverSocket_, (struct sockaddr*)&clientAddr, &clientLen);
		if (clientSocket < 0)
			throw std::runtime_error("Error: accept failed");
		if(fcntl(clientSocket, F_SETFL, O_NONBLOCK) < 0)
			throw std::runtime_error("Error: fcntl failed, cannot connect client");
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