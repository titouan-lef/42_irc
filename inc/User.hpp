#pragma once

#include "Client.hpp"
#include "sys/socket.h"

class User: public Client
{
public:

	User(int clientSocket, struct sockaddr_in clientAddr);
	// ~User();

	bool is(std::string nickname);

	const std::string& getNickname() const;
	const std::string& getUsername() const;
	const std::string getPrefix() const;
	int getSocket();
	struct sockaddr_in getAddress();

private:

	std::string nickname_;
	std::string username_;
	std::string realName_;

	int clientSocket_; // manage link with pollfds
	struct sockaddr_in clientAddr_;
	// Role role_;

	std::string buffer_;
};