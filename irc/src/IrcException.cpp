#include "IrcException.hpp"

/***************************************************************************************************************************************************/
#include <iomanip>
#include <sstream>

#include "UserManager.hpp"

IrcException::IrcException(const int socket, const int code, const std::string& trailing, const std::vector<std::string>& arg): socket_(socket)
{
	clientNickname_ = UserManager::getInstance().getUserNickname(socket);
	initIrcException(code, trailing, arg);
}

IrcException::IrcException(const int socket, const int code, const std::string& trailing, const std::vector<std::string>& arg,
	const std::string& clientNickname): socket_(socket), clientNickname_(clientNickname)
{
	initIrcException(code, trailing, arg);
}

IrcException::~IrcException() throw() {}

void IrcException::sendError() const
{
	MsgServ::getInstance().sendNumericReply(socket_, cmd_, clientNickname_);
	displayError();
}

void IrcException::initIrcException(const int code, const std::string& trailing, const std::vector<std::string>& arg)
{
	std::stringstream codeName;
	codeName << std::setw(3) << std::setfill('0') << code;
	cmd_.name = codeName.str();
	cmd_.trailing = trailing;
	cmd_.arg = arg;
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
#include "ChannelPrivilegesNeeded.hpp"
#include "NicknameInUseException.hpp"
#include "ErroneUsNicknameException.hpp"
#include "NeedMoreParamsException.hpp"
#include "NoNicknameGivenException.hpp"
#include "AlreadyRegistredException.hpp"
#include "InviteOnlyException.hpp"
#include "NotRegisteredException.hpp"
#include "InvalidModeParamException.hpp"
#include "NoSuchChannelException.hpp"
#include "UnknownModeException.hpp"
#include "BadChanMaskException.hpp"

NoSuchNickException::NoSuchNickException(const int socket, const std::string& nickname):
	IrcException(socket, 401, "No such nick/channel", std::vector<std::string>(1, nickname)) {}

NoSuchChannelException::NoSuchChannelException(const int socket, const std::string& channelName):
	IrcException(socket, 403, "No such channel", std::vector<std::string>(1, channelName)) {}

UnknowCmdException::UnknowCmdException(const int socket, const std::string& commandName): 
	IrcException(socket, 421, "Unknow command", std::vector<std::string>(1, commandName)) {}

NoNicknameGivenException::NoNicknameGivenException(const int socket, const std::string& nickname):
	IrcException(socket, 431, "No nickname given", std::vector<std::string>(), nickname) {}

ErroneUsNicknameException::ErroneUsNicknameException(const int socket, const std::vector<std::string>& arg, const std::string& nickname):
	IrcException(socket, 432, "Erroneous nickname", arg, nickname) {}

NicknameInUseException::NicknameInUseException(const int socket, const std::vector<std::string>& arg, const std::string& nickname):
	IrcException(socket, 433, "Nickname is already in use", arg, nickname) {}

NotRegisteredException::NotRegisteredException(int socket, const std::string& nickname):
	IrcException(socket, 451, "You have not registered", std::vector<std::string>(), nickname) {}

NeedMoreParamsException::NeedMoreParamsException(const int socket, const std::string& commandName, const std::string& nickname):
	IrcException(socket, 461, "Not enough parameters", std::vector<std::string>(1, commandName), nickname) {}

AlreadyRegistredException::AlreadyRegistredException(const int socket, const std::string& nickname):
	IrcException(socket, 462, "Unauthorized command (already registered)", std::vector<std::string>(), nickname) {}

PasswordMismatchException::PasswordMismatchException(const int socket, const std::string& nickname):
	IrcException(socket, 464, "Password not given or incorrect", std::vector<std::string>(), nickname) {}

ChannelIsFullException::ChannelIsFullException(const int socket, const std::string& channelName):
	IrcException(socket, 471, "Cannot join channel (+l)", std::vector<std::string>(1, channelName)) {}

UnknownModeException::UnknownModeException(const int socket, const char& mode):
	IrcException(socket, 472, " :is unknown mode char to me", std::vector<std::string>(1, std::string(1, mode))) {}

InviteOnlyException::InviteOnlyException(const int socket, const std::string& channelName):
	IrcException(socket, 473, "You're not invited to join this channel", std::vector<std::string>(1, channelName)) {}

BadChanMaskException::BadChanMaskException(const int socket, const std::string& channelName):
	IrcException(socket, 476, "Bad Channel Mask",  std::vector<std::string>(1, channelName)) {}

ChannelPrivilegesNeeded::ChannelPrivilegesNeeded(const int socket, const std::string& channelName):
	IrcException(socket, 482, "You're not channel operator", std::vector<std::string>(1, channelName)) {}

ClientDisconnectedException::ClientDisconnectedException(const int socket, const std::string& nickname):
	IrcException(socket, 504, "disconnected", std::vector<std::string>(), nickname) {}

ReiceveException::ReiceveException(const int socket, const std::string& nickname):
	IrcException(socket, 1, "Impossible to receive data", std::vector<std::string>(), nickname) {}

SendException::SendException(const int socket, const std::vector<std::string>& arg, const std::string& nickname):
	IrcException(socket, 1, "Impossible to send data from " + nickname, arg, nickname) {}

InvalidModeParamException::InvalidModeParamException(const int socket, const std::vector<std::string>& arg, const std::string& nickname):
	/** @todo check this*/ IrcException(socket, 696, "Invalid parameter for mode" , arg, nickname) {}