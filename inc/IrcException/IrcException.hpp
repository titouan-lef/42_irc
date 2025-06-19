#pragma once

#include <iostream>
#include "utils.hpp"

class IrcException
{
public:
	IrcException(const CommandResponse& CmdResponse);
	virtual ~IrcException() = 0;

	virtual const char* what() const throw();

private:
	const std::string message_;
};