#include "Channel.hpp"

#include "MsgServ.hpp"
#include "UserManager.hpp"
#include "BadChanKeyException.hpp"
#include "ChannelIsFullException.hpp"
#include "ChannelPrivilegesNeededException.hpp"
#include "InviteOnlyException.hpp"
#include "UserNotInChannelException.hpp"
#include "UserOnChannelException.hpp"
#include "NotOnChannelException.hpp"
#include "command.hpp"

#include <sstream>

Channel::Channel(std::string name, const int socketOperator): name_(name), nbMaxUsers_(0), inviteRestricted_(false), topicRestricted_(false)
{
	socketOperators_.push_back(socketOperator);
	connectUser(socketOperator);
}

void Channel::sendChannelSummary(const int socket) const
{
	std::vector<std::string> arg;
	std::string nbUserArg = sizeToStr(socketConnectedUsers_.size());
	arg.push_back(name_);
	arg.push_back(nbUserArg);
	if (topic_.empty())
		MsgServ::getInstance().sendNumericReply(socket, 322, arg);
	else
		MsgServ::getInstance().sendNumericReply(socket, 322, arg, topic_);
}

void Channel::sendActualTopic(const int socket) const
{
	if (topic_.empty())
		MsgServ::getInstance().sendNumericReply(socket, 331, std::vector<std::string>(1, name_), "No topic is set");
	else
		MsgServ::getInstance().sendNumericReply(socket, 332, std::vector<std::string>(1, name_), topic_);
}

void Channel::sendActualUsers(const int socket) const
{
	std::vector<std::string> arg;
	arg.push_back("=");
	arg.push_back(name_);
	MsgServ::getInstance().sendNumericReply(socket, 353, arg, builtUsersString());

	arg.erase(arg.begin());
	MsgServ::getInstance().sendNumericReply(socket, 366, arg, "End of NAMES list");
}

void Channel::sendChangedModes(const int socket, const std::string& options) const
{
	Command cmd;
	cmd.name = "MODE";
	cmd.arg.push_back(name_);
	cmd.arg.push_back(options);
	cmd.hasTrailing = false;
	manageCmdMessage(socket, cmd);
}

void Channel::sendActualModes(const int socket) const
{
	std::vector<std::string> arg;
	arg.push_back(name_);
	arg.push_back(builtOptionString());
	MsgServ::getInstance().sendNumericReply(socket, 324, arg);
}

void Channel::sendMessageToAllUsers(const int senderSocket, const Command& cmd) const
{
	for (size_t i = 0; i < socketConnectedUsers_.size(); i++)
	{
		if (senderSocket == socketConnectedUsers_.at(i))
			continue ;
		MsgServ::getInstance().sendMessageReply(senderSocket, socketConnectedUsers_.at(i), cmd);
	}
}

/**
 * @brief Adds a user to the list of connected users in the channel.
 * @param socketNewUser Socket of the user trying to log in.
 * @param password Password used by the user to log in.
 * @throws UserOnChannelException, BadChanKeyException, InviteOnlyException, ChannelIsFullException.
 */
void Channel::connectUser(int socketNewUser, const std::string& password)
{
	if (isConnected(socketNewUser))
	{
		std::string username = UserManager::getInstance().findUser(socketNewUser).getUsername();
		throw UserOnChannelException(username, name_);
	}
	if (isPasswordProtected() && password != password_)
		throw BadChanKeyException(name_);
	if (isInviteRestricted() && !isInvited(socketNewUser))
		throw InviteOnlyException(name_);
	if (isFull())
		throw ChannelIsFullException(name_);
	if (isInvited(socketNewUser))
		socketInvitedUsers_.erase(socketNewUser);
	socketConnectedUsers_.push_back(socketNewUser);
	manageJoinMessage(socketNewUser);
}

/**
 * @param socketUser Socket of the user who must be disconnected.
 * @throws UserNotInChannelException.
 */
bool Channel::disconnectUser(const int socketUser)
{
	removeOperator(socketUser);
	removeConnectedUser(socketUser);
	return (socketConnectedUsers_.empty());
}

/**
 * @brief Invite user to join the channel.
 * @param invitedSocket Socket of the user invited on the channel.
 * @param socketSender Socket of the user who is inviting.
 * @throws ChannelPrivilegesNeededException, UserOnChannelException.
 */
void Channel::inviteUser(int invitedSocket, int socketSender)
{
	if (!isConnected(socketSender))
		throw NotOnChannelException(name_);
	if (isInviteRestricted() && !isOperator(socketSender))
		throw ChannelPrivilegesNeededException(name_);
	if (isConnected(invitedSocket))
	{
		const std::string username = UserManager::getInstance().findUserNickname(invitedSocket);
		throw UserOnChannelException(username, name_);
	}
	socketInvitedUsers_.insert(invitedSocket);
}

/**
 * @brief Send message on channel when a user part of this one.
 * @param socketUser Socket of the user who part the channel.
 * @param message Message which must be send.
 * @throws UserNotInChannelException.
 */
void Channel::managePartMessage(const int socketUser, const char* message) const
{
	if (!isConnected(socketUser))
		throw NotOnChannelException(name_);
	Command cmd;
	cmd.name = "PART";
	cmd.arg.push_back(name_);
	cmd.hasTrailing = (message != NULL);
	if (cmd.hasTrailing)
		cmd.arg.push_back(message);
	manageCmdMessage(socketUser, cmd);
}

void Channel::manageKickMessage(const int socketUser, const int loggedOutSocket, const std::string& message) const
{
	if (!isConnected(loggedOutSocket))
	{
		std::string nickname = UserManager::getInstance().findUserNickname(loggedOutSocket);
		throw UserNotInChannelException(nickname, name_);
	}
	const std::string username = UserManager::getInstance().findUserName(loggedOutSocket);
	Command cmd;
	cmd.name = "KICK";
	cmd.arg.push_back(name_);
	cmd.arg.push_back(username);
	cmd.hasTrailing = true;
	cmd.arg.push_back(message);
	manageCmdMessage(socketUser, cmd);
}

/**
 * @return false if user is already an operator, true otherwise.
 */
bool Channel::addNewOperator(const int socketNewOperator)
{
	if (isOperator(socketNewOperator))
		return false;
	socketOperators_.push_back(socketNewOperator);
	return true;
}

/**
 * @return false if user is not an operator, true otherwise.
 */
bool Channel::removeOperator(const int socketUser)
{
	if (!isOperator(socketUser))
		return false;
	eraseUser(socketOperators_, socketUser);
	return true;
}

const std::string& Channel::getName() const { return name_; }

bool Channel::is(const std::string& name) const { return name_ == name; }

bool Channel::isConnected(const int socketUser) const { return findUser(socketConnectedUsers_, socketUser) != socketConnectedUsers_.end(); }

void Channel::insertConnectedUsers(std::set<int>& list)
{
	for (std::size_t i = 0; i < socketConnectedUsers_.size(); ++i)
		list.insert(socketConnectedUsers_.at(i));
}

bool Channel::isOperator(const int socketUser) const { return findUser(socketOperators_, socketUser) != socketOperators_.end(); }

bool Channel::isPasswordProtected() const { return !password_.empty(); }

/**
 * @brief Disables the channel password.
 * @param password Current password.
 * @return true if password use is the current password, false otherwise.
 * @note Ensure the user has operator privileges.
 */
bool Channel::disablePassword(const std::string& password)
{
	if (password != password_)
		return false;
	password_.clear();
	return true;
}

/**
 * @return false if password is already defined, true otherwise.
 * @note Ensure the user has operator privileges.
 */
bool  Channel::setPassword(const std::string& newPassword)
{
	if (isPasswordProtected())
		return false;
	password_ = newPassword;
	return true;
}

/**
 * @return false if limit is not defined, true otherwise.
 * @note Ensure the user has operator privileges.
 */
bool Channel::disableLimitNbUsers()
{
	if (nbMaxUsers_ == 0)
		return false;
	nbMaxUsers_ = 0;
	return true;
}

/**
 * @return false if limit is not defined or new limit is invalid, true otherwise.
 * @note Ensure the user has operator privileges.
 */
bool Channel::setLimitNbUsers(const std::size_t newNbMaxUsers)
{
	if (nbMaxUsers_ != 0 || newNbMaxUsers <= 0)
		return false;
	nbMaxUsers_ = newNbMaxUsers;
	return true;
}

/**
 * @return false if invitation are already restricted, true otherwise.
 * @note Ensure the user has operator privileges.
 */
bool Channel::enableInviteRestriction()
{
	if (isInviteRestricted())
		return false;
	inviteRestricted_ = true;
	return true;
}

/**
 * @return false if invitation are not restricted, true otherwise.
 * @note Ensure the user has operator privileges.
 */
bool Channel::disableInviteRestriction()
{
	if (!isInviteRestricted())
		return false;
	inviteRestricted_ = false;
	return true;
}

/**
 * @return false if topic is already restricted, true otherwise.
 * @note Ensure the user has operator privileges.
 */
bool Channel::enableTopicRestriction()
{
	if (isTopicRestricted())
		return false;
	topicRestricted_ = true;
	return true;
}

/**
 * @return false if topic are not restricted, true otherwise.
 * @note Ensure the user has operator privileges.
 */
bool Channel::disableTopicRestriction()
{
	if (!isTopicRestricted())
		return false;
	topicRestricted_ = false;
	return true;
}

/**
 * @throws UserNotInChannelException, ChannelPrivilegesNeededException.
 */
void Channel::setTopic(const std::string& newTopic, const int socketUser)
{
	if (!isConnected(socketUser))
		throw NotOnChannelException(name_);
	if (isTopicRestricted() && !isOperator(socketUser))
		throw ChannelPrivilegesNeededException(name_);
	topic_ = newTopic;
}

void Channel::manageJoinMessage(const int socketNewUser) const
{
	Command cmd;
	cmd.name = "JOIN";
	cmd.arg.push_back(name_);
	cmd.hasTrailing = true;
	manageCmdMessage(socketNewUser, cmd);
	sendActualTopic(socketNewUser);
	sendActualUsers(socketNewUser);
}

void Channel::manageCmdMessage(const int socketUser, const Command& cmd) const
{
	sendMessageToAllUsers(socketUser, cmd);
	MsgServ::getInstance().sendMessageReply(socketUser, socketUser, cmd);
}

const std::string Channel::builtUsersString() const
{
	std::string users;
	
	for (std::size_t i = 0; i < socketConnectedUsers_.size(); ++i)
	{
		if (i != 0)
			users += " ";
		if (isOperator(socketConnectedUsers_[i]))
			users += "@";
		users += UserManager::getInstance().findUserNickname(socketConnectedUsers_[i]);
	}
	return users;
}

const std::string Channel::builtOptionString() const
{
	std::string options;
	std::string args;

	if (isInviteRestricted())
		options += "i";
	if (isPasswordProtected())
	{
		options += "k";
		args += " " + password_;
	}
	if (isTopicRestricted())
		options += "t";
	if (nbMaxUsers_ != 0)
	{
		options += "l";
		args += " " + sizeToStr(nbMaxUsers_);
	}
	if (!options.empty())
		options = "+" + options;
	return options + args;
}

const std::string Channel::sizeToStr(const std::size_t nb) const
{
	std::stringstream ss;
	ss << nb;
	return ss.str();
}

void Channel::removeConnectedUser(const int socketUser)
{
	eraseUser(socketConnectedUsers_, socketUser);
}

void Channel::eraseUser(std::vector<int> &socketArray, const int socketUser)
{
	std::vector<int>::iterator it = findUser(socketArray, socketUser);
	if (it != socketArray.end())
		socketArray.erase(it);
}

std::vector<int>::iterator Channel::findUser(std::vector<int>& socketArray, const int socketUser) const
{
	std::vector<int>::iterator it = socketArray.begin();
	for (; it < socketArray.end(); ++it)
	{
		if (*it == socketUser)
			break;
	}
	return it;
}

std::vector<int>::const_iterator Channel::findUser(const std::vector<int> &socketArray, const int socketUser) const
{
	std::vector<int>::const_iterator it = socketArray.begin();
	for (; it < socketArray.end(); ++it)
	{
		if (*it == socketUser)
			break;
	}
	return it;
}

bool Channel::isFull() const { return (nbMaxUsers_ != 0 && socketConnectedUsers_.size() >= nbMaxUsers_); }

bool Channel::isInvited(const int socketNewUser) const { return socketInvitedUsers_.find(socketNewUser) != socketInvitedUsers_.end(); }

bool Channel::isInviteRestricted() const { return inviteRestricted_; }

bool Channel::isTopicRestricted() const { return topicRestricted_; }