#pragma once

#include <iostream>
#include <exception>

class IrcException : public std::exception
{
public:
	IrcException(const int code, const std::string& name, const std::string& message);
	virtual ~IrcException() throw() = 0;

	virtual const char* what() const throw();

private:
	const std::string message_;
};