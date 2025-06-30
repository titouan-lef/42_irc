#pragma once

#include <iostream>
#include <exception>
#include <vector>

#include "MsgServ.hpp"

class IrcException : public std::exception
{
public:
	IrcException(const int socket, const int code, const std::string& trailing, const std::vector<std::string>& arg);
	IrcException(const int socket, const int code, const std::string& trailing, const std::vector<std::string>& arg, const std::string& clientNickname);
	virtual ~IrcException() throw() = 0;

	void sendError() const;

private:
	void initIrcException(const int code, const std::string& trailing, const std::vector<std::string>& arg);
	void displayError() const;

	int socket_;
	MsgServ::Command cmd_;
	std::string clientNickname_;
};