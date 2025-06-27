#include "IrcException.hpp"

/***************************************************************************************************************************************************/
#include <iomanip>
#include <sstream>

#include "CommandResponse.hpp"

IrcException::IrcException(int socket, int code, const std::string& trailing, const std::vector<std::string>& arg, const std::string& clientNickname):
	socket_(socket), clientNickname_(clientNickname)
{
	std::stringstream codeName;
	codeName << std::setw(3) << std::setfill('0') << code;
	cmd_.name = codeName.str();
	cmd_.trailing = trailing;
	cmd_.arg = arg;
}

IrcException::~IrcException() throw() {}

void IrcException::sendError() const
{
	MsgServ::sendNumericReply(socket_, cmd_, clientNickname_);
	displayError();
}

void IrcException::displayError() const
{
	const char* WHITE = "\033[0m";
	const char* RED = "\033[31m";
	const char* GREEN = "\033[32m";
	const char* BLUE = "\033[34m";
	const char* MAGENTA = "\033[35m";

	std::cerr << RED << "Exception " << cmd_.name
		<< WHITE << " throws by "
		<< MAGENTA << clientNickname_
		<< WHITE << " :"
		<< BLUE;
	for (std::size_t i = 0; i < cmd_.arg.size(); ++i)
		std::cerr << " " << cmd_.arg[i];
	if (cmd_.trailing != "")
		std::cerr << GREEN << " " << cmd_.trailing;
	std::cerr << WHITE << "\r\n";
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
#include "NotRegisteredException.hpp"
#include "InvalidModeParamException.hpp"

NoSuchNickException::NoSuchNickException(int socket, const std::vector<std::string>& arg, const std::string& nickname):
	IrcException(socket, 401, "No such nick/channel", arg, nickname) {}

UnknowCmdException::UnknowCmdException(int socket, const std::string& commandName, const std::string& nickname): 
	IrcException(socket, 421, "Unknow command", std::vector<std::string>(1, commandName), nickname) {}

NoNicknameGivenException::NoNicknameGivenException(int socket, const std::string& nickname):
	IrcException(socket, 431, "No nickname given", std::vector<std::string>(), nickname) {}

ErroneUsNicknameException::ErroneUsNicknameException(int socket, const std::vector<std::string>& arg, const std::string& nickname):
	IrcException(socket, 432, "Erroneous nickname", arg, nickname) {}

NicknameInUseException::NicknameInUseException(int socket, const std::vector<std::string>& arg, const std::string& nickname):
	IrcException(socket, 433, "Nickname is already in use", arg, nickname) {}

NotRegisteredException::NotRegisteredException(int socket, const std::string& nickname):
	IrcException(socket, 451, "You have not registered", std::vector<std::string>(), nickname) {}

NeedMoreParamsException::NeedMoreParamsException(int socket, const std::string& commandName, const std::string& nickname):
	IrcException(socket, 461, "Not enough parameters", std::vector<std::string>(1, commandName), nickname) {}

AlreadyRegistredException::AlreadyRegistredException(int socket, const std::string& nickname):
	IrcException(socket, 462, "Unauthorized command (already registered)", std::vector<std::string>(), nickname) {}

PasswordMismatchException::PasswordMismatchException(int socket, const std::string& nickname):
	IrcException(socket, 464, "Password not given or incorrect", std::vector<std::string>(), nickname) {}

ChannelIsFullException::ChannelIsFullException(int socket, const std::string& channelName, const std::string& nickname, const std::string& name):
	IrcException(socket, 471, "The channel " + name + " is full (+l)", std::vector<std::string>(1, channelName), nickname) {}

InviteOnlyException::InviteOnlyException(int socket, const std::string& channelName, const std::string& nickname):
	IrcException(socket, 473, "You're not invited to join this channel", std::vector<std::string>(1, channelName), nickname) {}

ChannelPrivilegesNeeded::ChannelPrivilegesNeeded(int socket, const std::string& channelName, const std::string& nickname):
	IrcException(socket, 482, "You're not channel operator", std::vector<std::string>(1, channelName), nickname) {}

ClientDisconnectedException::ClientDisconnectedException(int socket, const std::string& nickname):
	IrcException(socket, 504, "disconnected", std::vector<std::string>(), nickname) {}

ReiceveException::ReiceveException(int socket, const std::string& nickname):
	IrcException(socket, 1, "Impossible to receive data", std::vector<std::string>(), nickname) {}

SendException::SendException(int socket, const std::vector<std::string>& arg, const std::string& nickname):
	IrcException(socket, 1, "Impossible to send data from " + nickname, arg, nickname) {}

InvalidModeParamException::InvalidModeParamException(int socket, const std::string& msg, const std::string& nickname):
	/** @todo check this*/ IrcException(socket, 696, msg ,std::vector<std::string>(), nickname) {}