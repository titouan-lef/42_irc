#pragma once

#include <iostream>

class IrcException
{
public:
	IrcException(const int code, const std::string& name, const std::string& message);
	virtual ~IrcException() = 0;

	virtual const char* what() const throw();

private:
	const std::string message_;
};