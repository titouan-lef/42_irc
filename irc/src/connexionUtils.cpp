#include "connexionUtils.hpp"

#include <cstring>				// memset
#include <netdb.h>				// gethostbyname, getprotobyname, htons, htonl
#include <sys/socket.h>			// socket, bind, accept, listen
#include <unistd.h>				// close
#include <cstdlib>				// atoi

int parsePort(const std::string& port)
{
	const int MIN_PORT_VALUE = 1024; //Well Known Ports (0, 1023)
	const int MAX_PORT_VALUE = 65535;

	if (port.empty())
		throw std::runtime_error("Error: Invalid port");
	int portNumber = std::atoi(port.c_str());
	if (portNumber < MIN_PORT_VALUE || portNumber > MAX_PORT_VALUE)
		throw std::runtime_error("Error: Invalid port");
	return portNumber;
}

int createSocket()
{
	int serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // AF_INET : Address family set to IPv4, SOCK_STREAM : type of domain, IPPROTO_TCP : protocol TCP
	if (serverSocket < 0)
		throw std::runtime_error("Error: cannot open socket");
	return serverSocket;
}

void defineSocketOption(int serverSocket)
{
	// Set SO_REUSEADDR to avoid "address already in use" error on restart
	int opt = 1;
	int result = setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)); //allow bind() to reuse port already binded because linux block it a few moment.
	if (result < 0)
	{
		close(serverSocket);
		throw std::runtime_error("Error: setsockopt failed");
	}
}

struct sockaddr_in createAddress(int port)
{
	struct sockaddr_in serverAddr;
	std::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET; // Local network (localhost)
	//convert values between host and network byte order
	serverAddr.sin_port = htons(port); // h stand for host
	serverAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // corresponding to inet_pton("127.0.0.1")
	return serverAddr;
}