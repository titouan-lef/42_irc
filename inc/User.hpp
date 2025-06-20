
#pragma once

#include "Client.hpp"
#include "sys/socket.h"

class User: public Client
{
public:

	User(int clientSocket, struct sockaddr_in clientAddr);
	// ~User();

	const std::string& receiveMsg();
	void sendMsg(const std::string& msg);
	void privateMsg(User* target , const std::string& msg); // ptr on clien on string(nickname)
	bool is(std::string nickname);

	int getSocket();
	struct sockaddr_in getAddress();

	class ErrorReiceve : public std::exception
	{
		public:
			const char* what() const throw();
	};

	class ClientDisconnected : public std::exception
	{
		public:
			const char* what() const throw();
	};

private:

	std::string nickname_;
	std::string realName_;

	const int clientSocket_; // manage link with pollfds
	const struct sockaddr_in clientAddr_;
	// Role role_;

	std::string buffer_;
};