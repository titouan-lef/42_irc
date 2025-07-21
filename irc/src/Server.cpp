#include "Server.hpp"

#include <cerrno>
#include <cstring>				// memset
#include <netdb.h>				// gethostbyname, getprotobyname, htons, htonl
#include <sys/socket.h>			// socket, bind, accept, listen
#include <cstdlib>

#include "UserManager.hpp"
#include "IrcException.hpp"

#define MIN_PORT_VALUE 1023
#define MAX_PORT_VALUE 65535

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
	bindSocketOnAdress(serverSocket, (sockaddr*)&serverAddr);
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

bool Server::isCorrectPassword(const std::string& password) const
{
	return (password_ == password);
}

bool Server::isUp() const
{
	return running_;
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
	run();
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
	creationDate_ = getDate();
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
		throw std::runtime_error("Error: Invalid port");
	int portNumber = std::atoi(port.c_str());
	if (portNumber <= MIN_PORT_VALUE)
		throw std::runtime_error("Error: Invalid port");
	if (portNumber > MAX_PORT_VALUE)
		throw std::runtime_error("Error: Invalid port");
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
	if (poll_count < 0)// Interrupted by something else that a signal /** @todo keep because of errno not allowed in correction ?? */
	{
		running_ = false;
		if (errno != EINTR)
			std::cerr << "Error: poll" << std::endl;
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
		e.what(); /** @todo why is it alone ? */
	}
}