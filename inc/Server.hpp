
#pragma once

#include <poll.h>

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

	struct pollfd pollfd_;
	bool running_;
};