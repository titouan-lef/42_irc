
#pragma once

#include <netinet/in.h>

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
	void addNewUser(int clientSocket, struct sockaddr_in clientAddr);
	void listenClients();

	int serverSocket_; // manage link with pollfds
	struct sockaddr_in serverAddr_;
	bool running_;
};