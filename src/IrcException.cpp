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
#include "ClientDisconnectedException.hpp"
#include "ReiceveException.hpp"
#include "SendException.hpp"

NoSuchNickException::NoSuchNickException(const std::string& nickname): IrcException(401, "ERR_NOSUCHNICK", nickname + " :No such nick/channel") {}

ClientDisconnectedException::ClientDisconnectedException(const std::string& nickname): IrcException(504, "ERR_USERNOTONSERV", nickname + " disconnected") {}

ReiceveException::ReiceveException(const std::string& nickname): IrcException(555, "ERR_RECVFAILED", "Impossible to receive data from " + nickname) {}

SendException::SendException(const std::string& nickname): IrcException(556, "ERR_SENDFAILED", "Impossible to send data from " + nickname) {}
