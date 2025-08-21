#include "Server.hpp"

#include <sys/socket.h>			// socket, bind, accept, listen
#include <unistd.h>

#include "UserManager.hpp"
#include "IrcException.hpp"
#include "connexionUtils.hpp"

Server& Server::getInstance()
{
	static Server instance;
	return instance;
}

const std::string &Server::getCreationDate() const { return creationDate_; }

void Server::initServerConnection(std::string port, std::string password)
{
	int portNumber = parsePort(port);
	definePassword(password);
	int serverSocket = createSocket();
	defineSocketOption(serverSocket);
	struct sockaddr_in serverAddr = createAddress(portNumber);
	bindSocketOnAddress(serverSocket, (sockaddr*)&serverAddr);
	defineSocketOnlistening(serverSocket);
	UserManager::getInstance().initUserManager(serverSocket, serverAddr);
	std::cout << "Server listening on port " << port << " ..." << std::endl;
}
std::string Server::getDate() const
{
	std::time_t now = std::time(NULL);
	char* dt = std::ctime(&now);
	if (dt)
	{
		std::string date(dt);
		return date.substr(0, date.length() - 1);
	}
	return "";
}

void Server::definePassword(const std::string& password)
{
	parsePassword(password);
	password_ = password;
}

bool Server::isInvalidPassword(const std::string& password) const { return (password_ != password); }

bool Server::isUp() const { return running_; }

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
	run();
	return (0);
}

void Server::applySigInt()
{
	sigIntDetected_ = 1;
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
	closeServerConnection();
}

Server::Server(): creationDate_(getDate()), running_(false), sigIntDetected_(0) {}

/**
 * @brief Associates the socket with the local address.
 */
void Server::bindSocketOnAddress(int serverSocket, struct sockaddr* serverAddr)
{
	if (bind(serverSocket, serverAddr, sizeof(*serverAddr)) < 0)
	{
		close (serverSocket);
		throw std::runtime_error("Error: bind failed");
	}
}
/** 
 * @brief Listen the serverSocket for connection and limit the queue in incomming connection.
*/
void Server::defineSocketOnlistening(int serverSocket)
{
	if (listen(serverSocket, SOMAXCONN) < 0)
	{
		close (serverSocket);
		throw std::runtime_error("Error: listen failed");
	}
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
		throw std::runtime_error("Error: Use just ascii value from 33 to 126");
}

void Server::onEvent()
{
	int poll_count = UserManager::getInstance().getUsersEvent(); // 5 sec timeout
	if (poll_count < 0)// Signal detected or error poll execution
	{
		if (sigIntDetected_ == 0)
			std::cerr << "Error: poll" << std::endl;
		stop();
		return;
	}
	UserManager::getInstance().checkActivities();
	if (poll_count > 0)
		manageEvent();
}

void Server::manageEvent()
{
	try
	{
		UserManager::getInstance().checkNewConnections();
		UserManager::getInstance().listenClients();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
}