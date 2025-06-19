
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
#include <vector>
#include "User.hpp"

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

	const User& findUserBySocket(int socket);
	void ManageMsg(User user);

	class NoSuchUser : public std::exception
	{
		public:
			const char* what() const throw();
	};

private:

	void checkNewConnections();
	void addNewUser(int clientSocket, struct sockaddr_in clientAddr);
	void listenClients();

	int serverSocket_;
	bool running_;
	std::vector<User> userList;
	struct pollfd pollfds_[MAX_CLIENTS + 1];
	int nfds_;

};