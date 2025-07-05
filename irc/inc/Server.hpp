#pragma once

#include <netinet/in.h>
#include <iostream>

class CommandTester ;	/** @todo remove before push */
class PassCmdTester ;	/** @todo remove before push */
class NickCmdTester ;	/** @todo remove before push */

class Server
{
public:

	static Server& getInstance();

	bool isCorrectPassword(std::string password);
	int up(std::string port,std::string password);
	void stop();

private:

	Server();
	~Server(){}
	Server(const Server& other);
	Server& operator=(const Server& other);

	void run();
	void initServerConnection(std::string port,std::string password);
	int createSocket();
	void defineSocketOption(int serverSocket);
	struct sockaddr_in createAddress(int port);
	void bindSocketOnAdress(int serverSocket, struct sockaddr* serverAddr);
	void defineSocketOnlistening(int serverSocket);
	int parsePort(const std::string& port);
	void definePassword(std::string password);
	void parsePassword(const std::string& password);
	void parsePasswordChar(const unsigned char c);
	void onEvent();
	void manageEvent();
	void closeServerConnection();

	std::string	password_;
	bool running_;

	friend class CommandTester ;	/** @todo remove before push */
	friend class PassCmdTester ;	/** @todo remove before push */
	friend class NickCmdTester ;	/** @todo remove before push */
};