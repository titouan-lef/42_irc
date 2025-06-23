#include "CommandResponse.hpp"
#include <iomanip>
#include <sstream>

CommandResponse::CommandResponse(const int code, const std::string& name, const std::string& message): code(code), name(name), message(message) {}

const std::string CommandResponse::getCommandResponse() const
{
	std::stringstream ss;
	ss << std::setw(3) << std::setfill('0') << code;
	return (ss.str() + " " + name + "\n\t\"" + message + "\"");
}