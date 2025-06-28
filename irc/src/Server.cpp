#include "Server.hpp"

#include <iostream>
#include <unistd.h>
#include <cerrno>
#include <cstring>				// memset
#include <netdb.h>				// gethostbyname, getprotobyname, htons, htonl
#include <sys/socket.h>			// socket, bind, accept, listen
#include <cstdlib>

#include "UserManager.hpp"
#include "IrcException.hpp"
#include "PasswordMismatchException.hpp"

Server& Server::getInstance()
{
	static Server instance;
	return instance;
}

void Server::initServerConnection(std::string port,std::string password)
{
	int portNumber = parsePort(port);
	definePassword(password);
	int serverSocket = createSocket();
	defineSocketOption(serverSocket);
	struct sockaddr_in serverAddr = createAddress(portNumber);
	bindSocketOnAdress(serverSocket, (sockaddr*)&serverAddr);
	defineSocketOnlistening(serverSocket);
	UserManager::getInstance().initUserManager(serverSocket, serverAddr);
	std::cout << "Server listening on port " << port << " ..." << std::endl;
} 

void Server::definePassword(std::string password)
{
	parsePassword(password);
	password_ = password;
}

bool Server::isCorrectPassword(std::string password)
{
	std::cout << password_ << " : " << password_.size() << " vs " << password << " : " << password.size() << std::endl;
	return (password_ == password);
}

void Server::closeServerConnection()
{
	UserManager::getInstance().closeAllConnection();
	std::cout << std::endl << "Shutting down server..." << std::endl;
}

int Server::up(std::string port,std::string password)
{
	try
	{
		Server::getInstance().initServerConnection(port, password);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}
	getInstance().run();
	return (0);
}

void Server::run()
{
	running_ = true;
	while (running_)
		onEvent();
}

void Server::stop()
{
	running_ = false;
	closeServerConnection(); // isn't this can cause a pb if the server do thing beffore ending the while loop ?
}

Server::Server(): running_(false)
{
	// Initialize pollfd for listening socket
}

int Server::createSocket()
{
	int serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverSocket < 0)
		throw std::runtime_error("Error: cannot open socket");
	return serverSocket;
}

void Server::defineSocketOption(int serverSocket)
{
	// Set SO_REUSEADDR to avoid "address already in use" error on restart
	int opt = 1;
	int result = setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));
	if (result < 0)
		throw std::runtime_error("Error: setsockopt failed");
}

struct sockaddr_in Server::createAddress(int port)
{
	struct sockaddr_in serverAddr;
	std::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port); // h stant for host
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); // inet_pton("127.0.0.1")
	return serverAddr;
}

/**
 * @brief Associates the socket with the local address.
 */
void Server::bindSocketOnAdress(int serverSocket, struct sockaddr* serverAddr)
{
	if (bind(serverSocket, serverAddr, sizeof(*serverAddr)) < 0)
		throw std::runtime_error("Error: bind failed");
}

void Server::defineSocketOnlistening(int serverSocket)
{
	if (listen(serverSocket, SOMAXCONN) < 0) 
		throw std::runtime_error("Error: listen failed");
}

int Server::parsePort(const std::string& port)
{
	if (port.empty())
		throw std::runtime_error("Error: invalid port");
	int portNumber = std::atoi(port.c_str());
	return portNumber;
} 

void Server::parsePassword(const std::string& password)
{
	if (password.empty())
		throw std::runtime_error("Error: Password is empty");
	for (size_t i = 0; i < password.length(); ++i)
		parsePasswordChar(password[i]);
}

void Server::parsePasswordChar(const unsigned char c)
{
	if (c <= 32 || c > 126)
		throw std::runtime_error("Error: Use just ascii value 33 at 126");
}

void Server::onEvent()
{
	int poll_count = UserManager::getInstance().getNbUserEvent(); // 5 sec timeout
	if (poll_count == 0)
		return; // Timeout, no events, loop again
	if (poll_count > 0)
	{
		manageEvent();
		return;
	}
	if (errno != EINTR)// Interrupted by something else that a signal
	{
		running_ = false;
		std::cerr << "Error: poll" << std::endl;
	}
}

void Server::manageEvent()
{
	try
	{
		UserManager::getInstance().checkNewConnections();
		UserManager::getInstance().listenClients();
	}
	catch(const IrcException& e)
	{
		e.sendError();
	}
}