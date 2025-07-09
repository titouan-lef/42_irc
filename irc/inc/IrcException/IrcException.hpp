#pragma once

#include <iostream>
#include <exception>
#include <vector>

#include "MsgServ.hpp"

class IrcException : public std::exception
{
public:
	IrcException(const int code, const std::vector<std::string>& arg, const std::string& trailing);
	IrcException(const int code, const std::string& trailing);
	virtual ~IrcException() throw() = 0;

	void sendError(const int receiverSocket) const;

protected:
	std::vector<std::string> arg_;

private:
	void displayError(const int receiverSocket) const;

	const int code_;
	const std::string trailing_;
};