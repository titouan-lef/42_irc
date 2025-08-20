#include "IrcException.hpp"

/***************************************************************************************************************************************************/

#include "Server.hpp"
#include "MsgServ.hpp"

IrcException::IrcException(const int code, const std::vector<std::string> &arg, const std::string &trailing):
	arg_(arg), code_(code), trailing_(trailing) {}

IrcException::IrcException(const int code, const std::string &trailing):
	arg_(std::vector<std::string>()), code_(code), trailing_(trailing) {}

IrcException::~IrcException() throw() {}

void IrcException::sendError(const int receiverSocket) const
{
	MsgServ::getInstance().sendNumericReply(receiverSocket, code_, arg_, trailing_);
	displayError(receiverSocket);
}

void IrcException::displayError(const int receiverSocket) const
{
	if (!Server::getInstance().isUp())
		return;

	const char* WHITE = "\033[0m";
	const char* RED = "\033[31m";
	const char* GREEN = "\033[32m";
	const char* BLUE = "\033[34m";
	const char* MAGENTA = "\033[35m";

	std::cerr << RED << "Exception " << code_
		<< WHITE << " throws by "
		<< MAGENTA << receiverSocket
		<< WHITE << " :"
		<< BLUE;
	for (std::size_t i = 0; i < arg_.size(); ++i)
		std::cerr << " " << arg_.at(i);
	std::cerr << GREEN << " " << trailing_;
	std::cerr << WHITE << "\r\n";
}

/***************************************************************************************************************************************************/
#include "NoSuchNickException.hpp"
#include "ClientDisconnectedException.hpp"
#include "ReiceveException.hpp"
#include "SendException.hpp"
#include "PasswordMismatchException.hpp"
#include "ChannelIsFullException.hpp"
#include "UnknownCmdException.hpp"
#include "ChannelPrivilegesNeededException.hpp"
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
#include "BadChanKeyException.hpp"
#include "BadChanMaskException.hpp"
#include "UserNotInChannelException.hpp"
#include "NotOnChannelException.hpp"
#include "UserOnChannelException.hpp"
#include "NoTextToSendException.hpp"
#include "TooManyTargetsException.hpp"
#include "MessageTooLongException.hpp"
#include "NoRecipientException.hpp"

#include "User.hpp"

NoSuchNickException::NoSuchNickException(const std::string& nickname):
	IrcException(401, std::vector<std::string>(1, nickname), "No such nick/channel") {}

NoSuchChannelException::NoSuchChannelException(const std::string& channelName):
	IrcException(403, std::vector<std::string>(1, channelName), "No such channel") {}

TooManyTargetsException::TooManyTargetsException(const std::string& target):
	IrcException(407, std::vector<std::string>(1, target), "To many target") {}

NoRecipientException::NoRecipientException(const std::string& commandName):
	IrcException(411, std::vector<std::string>(1, commandName), "No recipient given") {}

NoTextToSendException::NoTextToSendException():
	IrcException(412, "No text to send") {}

UnknownCmdException::UnknownCmdException(const std::string& commandName): 
	IrcException(421, std::vector<std::string>(1, commandName), "Unknown command") {}

NoNicknameGivenException::NoNicknameGivenException():
	IrcException(431, "No nickname given") {}

ErroneUsNicknameException::ErroneUsNicknameException(const std::string& nickname):
	IrcException(432, std::vector<std::string>(1, nickname), "Erroneous nickname") {}

NicknameInUseException::NicknameInUseException(const std::string& nickname):
	IrcException(433, std::vector<std::string>(1, nickname), "Nickname is already in use") {}

UserNotInChannelException::UserNotInChannelException(const std::string& nickname, const std::string& channelName):
	IrcException(441, "They aren't on that channel")
{
	arg_.push_back(nickname);
	arg_.push_back(channelName);
}

NotOnChannelException::NotOnChannelException(const std::string& channelName):
	IrcException(442, std::vector<std::string>(1, channelName), "You're not on that channel") {}

UserOnChannelException::UserOnChannelException(const std::string& username, const std::string& channelName):
	IrcException(443, "is already on channel")
{
	arg_.push_back(username);
	arg_.push_back(channelName);
}

NotRegisteredException::NotRegisteredException():
	IrcException(451, std::vector<std::string>(1, User::getDefaultNickname()), "You have not registered") {}

NeedMoreParamsException::NeedMoreParamsException(const std::string& commandName):
	IrcException(461, std::vector<std::string>(1, commandName), "Not enough parameters") {}

AlreadyRegistredException::AlreadyRegistredException():
	IrcException(462, "Unauthorized command (already registered)") {}

PasswordMismatchException::PasswordMismatchException():
	IrcException(464, "Password not given or incorrect") {}

ChannelIsFullException::ChannelIsFullException(const std::string& channelName):
	IrcException(471, std::vector<std::string>(1, channelName), "Cannot join channel (+l)") {}

UnknownModeException::UnknownModeException(const char& mode):
	IrcException(472, std::vector<std::string>(1, std::string(1, mode)), "is unknown mode char to me") {}

InviteOnlyException::InviteOnlyException(const std::string& channelName):
	IrcException(473, std::vector<std::string>(1, channelName), "You're not invited to join this channel") {}

BadChanKeyException::BadChanKeyException(const std::string& channelName):
	IrcException(475,  std::vector<std::string>(1, channelName), "Cannot join channel (+k)") {}

BadChanMaskException::BadChanMaskException(const std::string& channelName):
	IrcException(476,  std::vector<std::string>(1, channelName), "Bad Channel Mask") {}

ChannelPrivilegesNeededException::ChannelPrivilegesNeededException(const std::string& channelName):
	IrcException(482, std::vector<std::string>(1, channelName), "You're not channel operator") {}

ClientDisconnectedException::ClientDisconnectedException():
	IrcException(504, "disconnected") {}

ReiceveException::ReiceveException():
	IrcException(1, "Impossible to receive data") {}

SendException::SendException(const std::vector<std::string>& arg, const std::string& nickname):
	IrcException(1, arg, "Impossible to send data from " + nickname) {}

MessageTooLongException::MessageTooLongException():
	IrcException(686, "Message send is too long") {}

InvalidModeParamException::InvalidModeParamException(const std::vector<std::string>& arg):
	IrcException(696, arg, "Invalid parameter for mode") {}