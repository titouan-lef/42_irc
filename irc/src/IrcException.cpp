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
#include "PasswordMismatchException.hpp"
#include "ChannelIsFullException.hpp"
#include "UnknowCmdException.hpp"
#include "PasswordMismatchException.hpp"
#include "ChannelIsFullException.hpp"
#include "ChannelPrivilegesNeeded.hpp"
#include "NicknameInUseException.hpp"
#include "ErroneUsNicknameException.hpp"
#include "NeedMoreParamsException.hpp"
#include "NoNicknameGivenException.hpp"
#include "AlreadyRegistredException.hpp"
#include "InviteOnlyException.hpp"

NoSuchNickException::NoSuchNickException(const std::string& nickname): IrcException(401, "ERR_NOSUCHNICK", nickname + " :No such nick/channel") {}

UnknowCmdException::UnknowCmdException(const std::string& nickname): IrcException(407, "ERR_UNKNOWCOMMAND", nickname + " :Unknow command") {}

NoNicknameGivenException::NoNicknameGivenException(): IrcException (431, "*", " :No nickname given"){}

ErroneUsNicknameException::ErroneUsNicknameException(const std::string& nickname): IrcException(432, "*" , nickname + " :Erroneous nickname"){}

NicknameInUseException::NicknameInUseException(const std::string& nickname): IrcException(433, "*" , nickname + " :Nickname is already in use"){}

NeedMoreParamsException::NeedMoreParamsException(const std::string & nickname): IrcException(461, "*", nickname + "PASS :Not enough parameters"){}

InviteOnlyException::InviteOnlyException(): IrcException(473, "ERR_INVITEONLYCHAN", "You're not invited to join this channel") {}

AlreadyRegistredException::AlreadyRegistredException(const std::string & nickname): IrcException(462, "", nickname + " :You may not reregister"){}

PasswordMismatchException::PasswordMismatchException(): IrcException(464, "ERR_PASSWDMISMATCH", "Password not given or incorrect") {}

ChannelIsFullException::ChannelIsFullException(const std::string& name): IrcException(471, "ERR_CHANNELISFULL", "The channel " + name + " is full") {}

ChannelPrivilegesNeeded::ChannelPrivilegesNeeded(): IrcException(482, "ERR_CHANOPRIVSNEEDED", "You're not channel operator") {}

ClientDisconnectedException::ClientDisconnectedException(const std::string& nickname): IrcException(504, "ERR_USERNOTONSERV", nickname + " disconnected") {}

ReiceveException::ReiceveException(const std::string& nickname): IrcException(1, "ERR_RECVFAILED", "Impossible to receive data from " + nickname) {}

SendException::SendException(const std::string& nickname): IrcException(1, "ERR_SENDFAILED", "Impossible to send data from " + nickname) {}
