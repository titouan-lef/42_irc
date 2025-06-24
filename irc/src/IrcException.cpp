#include "IrcException.hpp"

/***************************************************************************************************************************************************/
#include "CommandResponse.hpp"

IrcException::IrcException(const int code, const std::string& name, const std::string& message):
	message_(CommandResponse(code, name, message).getCommandResponse()) {}

IrcException::~IrcException() throw() {}

const char* IrcException::what() const throw()
{
	return message_.c_str();
}

/***************************************************************************************************************************************************/
#include "NoSuchNickException.hpp"
#include "ClientDisconnectedException.hpp"
#include "ReiceveException.hpp"
#include "SendException.hpp"
#include "PasswordDismatchException.hpp"
#include "ChannelIsFullException.hpp"
#include "UnknowCmdException.hpp"
#include "PasswordDismatchException.hpp"
#include "ChannelIsFullException.hpp"
#include "ChannelPrivilegesNeeded.hpp"

NoSuchNickException::NoSuchNickException(const std::string& nickname): IrcException(401, "ERR_NOSUCHNICK", nickname + " :No such nick/channel") {}

ClientDisconnectedException::ClientDisconnectedException(const std::string& nickname): IrcException(504, "ERR_USERNOTONSERV", nickname + " disconnected") {}

ReiceveException::ReiceveException(const std::string& nickname): IrcException(1, "ERR_RECVFAILED", "Impossible to receive data from " + nickname) {}

UnknowCmdException::UnknowCmdException(const std::string& nickname): IrcException(407, "ERR_UNKNOWCOMMAND", nickname + " :Unknow command") {}

SendException::SendException(const std::string& nickname): IrcException(1, "ERR_SENDFAILED", "Impossible to send data from " + nickname) {}

PasswordDismatchException::PasswordDismatchException(): IrcException(464, "ERR_PASSWDMISMATCH", "Password not given or incorrect") {}

ChannelIsFullException::ChannelIsFullException(const std::string& name): IrcException(471, "ERR_CHANNELISFULL", "The channel " + name + " is full") {}

ChannelPrivilegesNeeded::ChannelPrivilegesNeeded(): IrcException(482, "ERR_CHANOPRIVSNEEDED", "You're not channel operator") {}