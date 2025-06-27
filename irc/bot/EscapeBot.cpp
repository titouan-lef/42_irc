#include "EscapeBot.hpp"

#include <iostream>
#include <cstring>
#include <cassert>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdlib>

#define MIN_PORT_VALUE 1023
#define MAX_PORT_VALUE 65535

EscapeBot& EscapeBot::getInstance()
{
	static EscapeBot instance;
	return instance;
}

int EscapeBot::up(std::string port,std::string password)
{
	try
	{
		initServerConnection(port, password);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}
	run();
	return 0;
}

void EscapeBot::sendCommand(const std::string& cmd)
{
	std::string fullCmd = cmd + "\r\n";
	ssize_t sent = send(socketBot_, fullCmd.c_str(), fullCmd.length(), 0);
	if (sent < 0)
		std::cerr << "Error: Impossible to send the message" << std::endl;
	else if (size_t(sent) < fullCmd.size())
		std::cerr << "Error, the message sent is incomplete" << std::endl;
}

EscapeBot::EscapeBot(): socketBot_(-1), running_(false) {}

void EscapeBot::run()
{
	running_ = true;
	while (running_)
	{
		std::string test = listenToServer();
		std::cout << test << std::endl;
	}
}

void EscapeBot::initServerConnection(std::string port,std::string password)
{
	int portNumber = parsePort(port);
	socketBot_ = socket(AF_INET, SOCK_STREAM, 0);
	if(socketBot_ <= 0)
		throw std::runtime_error("Error: impossible to create the socket");
	int opt = 1;
	int result = setsockopt(socketBot_, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));
	if (result < 0)
		throw std::runtime_error("Error: setsockopt failed");
	sockaddr_in server_addr;
	std::memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(portNumber);
	server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

	int res = connect(socketBot_, (sockaddr*)&server_addr, sizeof(server_addr));
	if (res < 0)
		throw std::runtime_error("Error: impossible to connect to the server");

	sendCommand("PASS " + password);
	sendCommand("NICK escapeBot");
	// sendCommand("USER escapeBot 0 * :Master of the universe");
	running_ = true;
}

int EscapeBot::parsePort(const std::string& port)
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

std::string EscapeBot::listenToServer()
{
	char buf[512];
	std::memset(buf, 0, sizeof(buf));
	ssize_t received = recv(socketBot_, buf, sizeof(buf) - 1, 0);
	if (received < 0)
	{
		if (running_ == true)
		{
			std::cerr << "Error: problem with server communication." << std::endl;
			stop();
		}
	}
	if (received == 0) // Connection closed by server
	{
		std::cout << "Server disconnected." << std::endl;
		stop();
	}
	return buf;
}

void EscapeBot::stop()
{
	if (socketBot_ >= 0)
	{
		close(socketBot_);
		socketBot_ = -1;
	}
	running_ = false;
	std::cout << std::endl << "Shutting down EscapeBot..." << std::endl;
}
