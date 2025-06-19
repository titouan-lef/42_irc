#include "IrcException.hpp"

IrcException::IrcException(const CommandResponse& CmdResponse): message_(CmdResponse.getCommandResponse()) {}

IrcException::~IrcException() {}

const char* IrcException::what() const throw()
{
	return message_.c_str();
}