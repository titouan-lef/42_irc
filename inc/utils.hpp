#pragma once

#include <iostream>
#include <iomanip>
#include <sstream>

struct CommandResponse
{
public:
	CommandResponse(const int code, const std::string& name, const std::string& message)
	{
		this->code = code;
		this->name = name;
		this->message = message;
	}

	const std::string getCommandResponse() const
	{
		std::stringstream ss;
		ss << std::setw(3) << std::setfill('0') << code;
		return (ss.str() + " " + name + "\n\t\"" + message + "\"");
	}

	int code;
	std::string name;
	std::string message;
};
