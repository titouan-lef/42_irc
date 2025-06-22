
#pragma once

#include <netinet/in.h>
#include <iostream>

class Server
{
public:

	Server();
	// ~Server();

	void initServerConnection(int port);
	void initPassword(std::string password);
	void closeServerConnection();
	void run();
	void stop();

private:

	void checkNewConnections();
	void addNewUser(int clientSocket, struct sockaddr_in clientAddr);
	void listenClients();

	int serverSocket_; // manage link with pollfds
	struct sockaddr_in serverAddr_;
	std::string	password_;
	bool running_;
};