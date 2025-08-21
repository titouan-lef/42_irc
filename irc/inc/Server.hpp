#pragma once

#include <iostream>
#include <csignal>

class CommandTester ;	/** @todo remove before push */
class PassCmdTester ;	/** @todo remove before push */
class NickCmdTester ;	/** @todo remove before push */
class PrivMsgTester ;	/** @todo remove before push */

class Server
{
public:
	static Server& getInstance();
	
	const std::string& getCreationDate() const;
	bool isInvalidPassword(const std::string& password) const;
	bool isUp() const;
	int up(std::string port,std::string password);
	void applySigInt();
	
private:
	Server();
	
	void run();
	void stop();
	void initServerConnection(std::string port, std::string password);
	std::string getDate() const;
	void bindSocketOnAddress(int serverSocket, struct sockaddr* serverAddr);
	void defineSocketOnlistening(int serverSocket);
	void definePassword(const std::string& password);
	void parsePassword(const std::string& password);
	void parsePasswordChar(const unsigned char c);
	void onEvent();
	void manageEvent();
	void closeServerConnection();
	
	std::string	creationDate_;
	std::string	password_;
	bool running_;
	volatile sig_atomic_t sigIntDetected_;

	friend class CommandTester ;	/** @todo remove before push */
	friend class PassCmdTester ;	/** @todo remove before push */
	friend class NickCmdTester ;	/** @todo remove before push */
	friend class PrivMsgTester ;	/** @todo remove before push */
};