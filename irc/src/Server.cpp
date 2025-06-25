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

Server& Server::getInstance()
{
	static Server instance;
	return instance;
}

void Server::initServerConnection(int port)
{
	createSocket();
	defineSocketOption();
	struct sockaddr_in serverAddr = createAddress(port);
	bindSocketOnAdress((sockaddr*)&serverAddr);
	defineSocketOnlistening();
	UserManager::getInstance().initUserManager(serverSocket_, serverAddr);
	std::cout << "Server listening on port " << port << " ..." << std::endl;
}

void Server::definePassword(std::string password)
{
	parsePassword(password);
	password_ = password;
}

bool Server::isCorrectPassword(std::string password)
{
	return (password_ == password);
}

void Server::closeServerConnection()
{
	UserManager::getInstance().closeAllConnection();
	if (serverSocket_ != -1)
		close (serverSocket_);
	std::cout << "Shutting down server..." << std::endl;
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
}

Server::Server(): serverSocket_(-1), running_(false)
{
	// Initialize pollfd for listening socket
}

void Server::createSocket()
{
	serverSocket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverSocket_ < 0)
		throw std::runtime_error("Error: cannot open socket");
}

void Server::defineSocketOption()
{
	// Set SO_REUSEADDR to avoid "address already in use" error on restart
	int opt = 1;
	int result = setsockopt(serverSocket_, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));
	if (result < 0)
		throw std::runtime_error("Error: setsockopt failed");
}

struct sockaddr_in Server::createAddress(int port)
{
	struct sockaddr_in serverAddr;
	std::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	return serverAddr;
}

/**
 * @brief Associates the socket with the local address.
 */
void Server::bindSocketOnAdress(struct sockaddr* serverAddr)
{
	if (bind(serverSocket_, serverAddr, sizeof(*serverAddr)) < 0)
		throw std::runtime_error("Error: bind failed");
}

void Server::defineSocketOnlistening()
{
	if (listen(serverSocket_, SOMAXCONN) < 0) 
		throw std::runtime_error("Error: listen failed");
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
		checkNewConnections(); // Check if listening socket is readable => new connection
		UserManager::getInstance().listenClients();
	}
	catch(const IrcException& e)
	{
		std::cerr << e.what() << std::endl;
	}
}

void Server::checkNewConnections()
{
	if (!UserManager::getInstance().serverReiceveData())
		return;
	struct sockaddr_in clientAddr;
	socklen_t clientLen = sizeof(clientAddr);
	int clientSocket = accept(serverSocket_, (struct sockaddr*)&clientAddr, &clientLen);
	if (clientSocket < 0)
		throw std::runtime_error("Error: accept failed");
	if(fcntl(clientSocket, F_SETFL, O_NONBLOCK) < 0)
		throw std::runtime_error("Error: fcntl failed, cannot connect client");
	addNewUser(clientSocket, clientAddr);
}

void Server::addNewUser(int clientSocket, struct sockaddr_in clientAddr)
{
	UserManager::getInstance().addNewUser(clientSocket, clientAddr);
	std::cout << "Accepted connection from " << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port) << std::endl;
}