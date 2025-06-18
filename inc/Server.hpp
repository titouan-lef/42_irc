
#pragma once

#include <iostream>
#include <csignal>
#include <cerrno>
#include <cstring>				// memset
#include <cstdlib>				// atoi
#include <unistd.h>				// close
#include <arpa/inet.h>			// inet_ntoa
#include <fcntl.h>
#include <netdb.h>				// gethostbyname, getprotobyname, htons, htonl
#include <sys/socket.h>			// socket, bind, accept, listen
#include <poll.h>				// poll

#define MAX_CLIENTS 100

class Server
{
public:

	Server();
	// ~Server();

	void initServerConnection(int port);
	void closeServerConnection();
	void run();
	void stop();

private:

	void checkNewConnections();
	void listenClients();

	int serverSocket;
	bool running;
	struct pollfd pollfds[MAX_CLIENTS + 1];
	int nfds;

};