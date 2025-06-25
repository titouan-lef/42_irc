
#pragma once

#include <netinet/in.h>
#include <iostream>

class Server
{
public:

	static Server& getInstance();

	void initServerConnection(int port);
	void initPassword(std::string password);
	const std::string& getPassword();
	void closeServerConnection();
	void run();
	void stop();

private:

	Server();
	~Server(){}
	Server(const Server& other);
	Server& operator=(const Server& other);

	void checkNewConnections();
	void addNewUser(int clientSocket, struct sockaddr_in clientAddr);
	void listenClients();

	int serverSocket_; // manage link with pollfds
	struct sockaddr_in serverAddr_;
	std::string	password_;
	bool running_;
};