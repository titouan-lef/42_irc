#pragma once

#include <iostream>

struct CommandResponse
{
public:
	CommandResponse(const int code, const std::string& name, const std::string& message);

	const std::string getCommandResponse() const;

	int code;
	std::string name;
	std::string message;
};