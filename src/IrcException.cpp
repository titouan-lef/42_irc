#include "IrcException.hpp"

/***************************************************************************************************************************************************/
#include "CommandResponse.hpp"

IrcException::IrcException(const int code, const std::string& name, const std::string& message):
	message_(CommandResponse(code, name, message).getCommandResponse()) {}

IrcException::~IrcException() {}

const char* IrcException::what() const throw()
{
	return message_.c_str();
}

/***************************************************************************************************************************************************/
#include "NoSuchNickException.hpp"

NoSuchNickException::NoSuchNickException(const std::string& nickname): IrcException(401, "ERR_NOSUCHNICK", nickname + " :No such nick/channel") {}