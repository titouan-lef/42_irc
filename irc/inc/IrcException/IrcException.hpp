#pragma once

#include <iostream>
#include <exception>

#include "MsgServ.hpp"

class IrcException : public std::exception
{
public:
	IrcException(int socket, int code, const std::string& trailing, const std::vector<std::string>& arg, const std::string& clientNickname);
	virtual ~IrcException() throw() = 0;

	void sendError() const;

private:
	int socket_;
	MsgServ::Command cmd_;
	std::string clientNickname_;
};