
#pragma once

#include "Client.hpp"
#include "sys/socket.h"

class User: public Client
{
public:

	User(int clientSocket, struct sockaddr_in clientAddr);
	// ~User();

	bool is(std::string nickname);

	std::string getNickname();
	int getSocket();
	struct sockaddr_in getAddress();

private:

	std::string nickname_;
	std::string realName_;

	int clientSocket_; // manage link with pollfds
	struct sockaddr_in clientAddr_;
	// Role role_;

	std::string buffer_;
};