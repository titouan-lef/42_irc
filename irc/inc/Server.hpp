#pragma once

#include <netinet/in.h>
#include <iostream>

class Server
{
public:

	static Server& getInstance();

	void initServerConnection(int port);
	void definePassword(std::string password);
	bool isCorrectPassword(std::string password);
	void closeServerConnection();
	void run();
	void stop();

private:

	Server();
	~Server(){}
	Server(const Server& other);
	Server& operator=(const Server& other);

	void createSocket();
	void defineSocketOption();
	struct sockaddr_in createAddress(int port);
	void bindSocketOnAdress(struct sockaddr* serverAddr);
	void defineSocketOnlistening();
	void parsePassword(const std::string& password);
	void parsePasswordChar(const unsigned char c);
	void onEvent();
	void manageEvent();
	void checkNewConnections();
	void addNewUser(int clientSocket, struct sockaddr_in clientAddr);

	int serverSocket_; // manage link with pollfds
	std::string	password_;
	bool running_;
};