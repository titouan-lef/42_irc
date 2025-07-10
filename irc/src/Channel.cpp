#include "Channel.hpp"

#include "UserManager.hpp"
#include "NoSuchNickException.hpp"
#include "BadChanKeyException.hpp"
#include "ChannelIsFullException.hpp"
#include "ChannelPrivilegesNeeded.hpp"
#include "InviteOnlyException.hpp"
#include "InvalidModeParamException.hpp"
#include "UserNotInChannelException.hpp"
#include "UserOnChannelException.hpp"

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

	MsgServ::getInstance().sendNumericReply(socket, 366, std::vector<std::string>(1, name_), "End of NAMES list");
}

void Channel::sendActualModes(const int socket) const
{
	std::vector<std::string> arg;
	arg.push_back(name_);
	arg.push_back(builtOptionString());
	MsgServ::getInstance().sendNumericReply(socket, 324, arg);
}

/**
 * @brief Sends a message to all users connected to the channel.
 *
 * @param cmd The command structure with name, trailing and agrs.
 */
void Channel::sendMessageToAllUsers(const int senderSocket, const std::string &cmdName, const std::vector<std::string> &arg, const std::string &trailing) const
{
	for (size_t i = 0; i < socketConnectedUsers_.size(); i++)
	{
		if (senderSocket == socketConnectedUsers_.at(i))
			continue ;
		MsgServ::getInstance().sendMessageReply(senderSocket, socketConnectedUsers_.at(i), cmdName, arg, trailing);
	}
}

/**
 * @brief Adds a user to the list of connected users in the channel.
 *
 * @param newUser Reference to the user being connected.
 * @param password Reference to the password the user is trying to connect with.
 * 
 * @throws PasswordMismatchException if the given password is incorect.
 * @throws ChannelIsFullException if the maximum number of users is reached.
 */
void Channel::connectUser(int socketNewUser, const std::string& password)
{
	if (isConnected(socketNewUser))
	{
		std::string username = UserManager::getInstance().getUser(socketNewUser).getUsername();
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
	/** @todo send message to all user about new connection (refacto needed) */
}


/**
 * @brief Removes a user from the list of connected users by nickname.
 *
 * @param nickname The nickname of the user to disconnect.
 *
 * @throws NoSuchNickException If no user with the given nickname is found in the channel.
 */
int Channel::disconnectUser(const int socketUser, const std::string& message)
{
	if (!isConnected(socketUser))
	{
		const std::string nickname = UserManager::getInstance().getUserNickname(socketUser);
		throw UserNotInChannelException(nickname, name_);
	}
	managePartMessage(socketUser, message);
	removeOperator(socketUser);
	removeConnectedUser(socketUser);
	if (socketConnectedUsers_.size() == 0)
		return -1;
	return 0;
}


/**
 * @brief Removes a user from the list of connected users by nickname.
 *
 * @param nickname The nickname of the user to disconnect.
 *
 * @throws NoSuchNickException If no user with the given nickname is found in the channel.
 */
int Channel::disconnectUser(const int socketUser, const int senderSocket, const std::string& message)
{
	if (!isConnected(socketUser))
	{
		const std::string nickname = UserManager::getInstance().getUserNickname(socketUser);
		throw UserNotInChannelException(nickname, name_);
	}
	std::string username = UserManager::getInstance().getUserName(socketUser);
	manageKickMessage(senderSocket, username, message);
	removeOperator(socketUser);
	removeConnectedUser(socketUser);
	if (socketConnectedUsers_.size() == 0)
		return -1;
	return 0;
}

/**
 * @brief Invite user to join the channel.
 *
 * @param invitedSocket The socket of the user invited on the channel.
 * @param socketSender The socket os the user who is inviting.
 * 
 * @throw ChannelPrivilegesNeeded if socketSender is not operator.
 * @throw UserOnChannelException if invitedSocket is already connected on the channel.
 */
void Channel::inviteUser(int invitedSocket, int socketSender)
{
	if (!isOperator(socketSender))
		throw ChannelPrivilegesNeeded(name_);
	if (isConnected(invitedSocket))
	{
		const std::string username = UserManager::getInstance().getUserNickname(invitedSocket);
		throw UserOnChannelException(username, name_);
	}
	socketInvitedUsers_.insert(invitedSocket);
}

/**
 * @brief Adds a new operator to the channel.
 *
 * @param newOperator Reference to the user to be added as operator.
 *
 * @return -1 if the user is not connected to the channel, 0 otherwise.
 * 
 * @note You must throw UserNotInChannelException if it returns -1.
 */
void Channel::addNewOperator(const int socketNewOperator)
{
	socketOperators_.push_back(socketNewOperator);
}

/**
 * @brief Removes a operator from the channel by nickname.
 *
 * @param nickname The nickname of the operator to remove.
 *
 * @throws NoSuchNickException if no operator with the given nickname is found.
 */
void Channel::removeOperator(const int socketUser)
{
	eraseUser(socketOperators_, socketUser);
}

/**
 * @brief Get the name of the channel.
 *
 * @return A constant reference to the channel's name.
 */
const std::string& Channel::getName() const { return name_; }

bool Channel::is(const std::string& name) const { return name_ == name; }

/**
 * @brief Checks if a user is a channel operator by nickname.
 *
 * @param nickname The nickname to check.
 *
 * @return True if the user is a operator, false otherwise.
 */
bool Channel::isConnected(const int socketUser) const { return findUser(socketConnectedUsers_, socketUser) != socketConnectedUsers_.end(); }

/**
 * @brief Checks if a user is a channel operator by nickname.
 *
 * @param nickname The nickname to check.
 *
 * @return True if the user is a operator, false otherwise.
 */
bool Channel::isOperator(const int socketUser) const { return findUser(socketOperators_, socketUser) != socketOperators_.end(); }

/**
 * @brief Check if a password is needed to access the channel.
 *
 * @return true if the channel is password protected, false otherwise.
 */
bool Channel::isPasswordProtected() const { return !password_.empty(); }

/**
 * @brief Disables the channel password.
 */
void Channel::disablePassword() { password_.clear(); }

/**
 * @brief Sets a new password for the channel.
 *
 * @param newPassword The new password to protect the channel.
 */
void  Channel::setPassword(const std::string& newPassword) { password_ = newPassword; }

/**
 * @brief Disables the user limit on the channel.
 *
 * Sets the maximum user count to -1, meaning no limit.
 */
void Channel::disableLimitNbUsers() { nbMaxUsers_ = 0; }

/**
 * @brief Sets the maximum number of users allowed in the channel.
 *
 * @param nbUserMax The maximum user count to set.
 * 
 * @return 1 if the given nbUserMax is below 1, 0 otherwise.
 * 
 * @warning you need to throw InvalidModeParamException if it returns 1.
 */
int Channel::setLimitNbUsers(size_t nbUserMax)
{
	if (nbUserMax <= 0)
		return 1;
	nbMaxUsers_ = nbUserMax;
	return 0;
}

/**
 * @brief Restrict connection to invited users only.
 *
 * @note This function should be called by the MODE command.
 *	   Ensure the user has operator privileges before applying this mode.
 */
void Channel::enableInviteRestriction() { inviteRestricted_ = true; }

/**
 * @brief Allow all users to join the channel.
 *
 * @note This function should be called by the MODE command.
 *	   Ensure the user has operator privileges before applying this mode.
 */
void Channel::disableInviteRestriction() { inviteRestricted_ = false; }

/**
 * @brief Restrict topic changes to channel operators only.
 *
 * @note This function should be called by the MODE command.
 *	   Ensure the user has operator privileges before applying this mode.
 */
void Channel::enableTopicRestriction() { topicRestricted_ = true; }

/**
 * @brief Allow all users to change the channel topic.
 *
 * @note This function should be called by the MODE command.
 *	   Ensure the user has operator privileges before applying this mode.
 */
void Channel::disableTopicRestriction() { topicRestricted_ = false; }

/**
 * @brief Sets the channel's topic to a new value.
 *
 * @param newTopic The new topic string to assign to the channel.
 * @param user The user trying to change the topic.
 * 
 * @throws ChannelPrivilegesNeeded is the topic is restricted to operators
 *		 and that the given user is not moderateur.
 */
void Channel::setTopic(const std::string& newTopic, const int socketUser)
{
	if (!isConnected(socketUser))
		throw UserNotInChannelException(UserManager::getInstance().getUserNickname(socketUser), name_);
	if (isTopicRestricted() && !isOperator(socketUser))
		throw ChannelPrivilegesNeeded(name_);
	topic_ = newTopic;
}

void Channel::manageJoinMessage(const int socketNewUser) const
{
	manageCmdMessage(socketNewUser, "JOIN", std::vector<std::string>(), name_);
	sendActualTopic(socketNewUser);
	sendActualUsers(socketNewUser);
}

void Channel::managePartMessage(const int socketUser, const std::string &message) const
{
	manageCmdMessage(socketUser, "PART", std::vector<std::string>(1, name_), message);
}

void Channel::manageKickMessage(const int socketUser, const std::string &username, const std::string &message) const
{
	std::vector<std::string> arg;
	arg.push_back(name_);
	arg.push_back(username);
	manageCmdMessage(socketUser, "KICK", arg, message);
}

void Channel::manageCmdMessage(const int socketUser, const std::string &cmdName, const std::vector<std::string> &arg, const std::string &message) const
{
	sendMessageToAllUsers(socketUser, cmdName, arg, message);
	MsgServ::getInstance().sendMessageReply(socketUser, socketUser, cmdName, arg, message);
}

/**
 * @brief Sends a numeric to all users connected to the channel.
 *
 * @param cmd The command structure with name, trailing and agrs.
 */
/*void Channel::sendNumericToAllUsers(const Command& cmd)
{
	(void) cmd;
	for (std::vector<User>::iterator it = connectedUser_.begin(); it < connectedUser_.end(); ++it)
	{
		MsgServ::sendNumericReply(it->getSocket(), cmd, it->getNickname());
	}
}*/

const std::string Channel::builtUsersString() const
{
	std::string users;
	
	for (std::size_t i = 0; i < socketConnectedUsers_.size(); ++i)
	{
		if (i != 0)
			users += " ";
		if (isOperator(socketConnectedUsers_[i]))
			users += "@";
		users += UserManager::getInstance().getUserNickname(socketConnectedUsers_[i]);
	}
	return users;
}

const std::string Channel::builtOptionString() const
{
	std::string options;

	if (isInviteRestricted())
		options += "i";
	if (isPasswordProtected())
		options += "k";
	if (isTopicRestricted())
		options += "t";
	if (nbMaxUsers_ != 0)
		options += "l " + sizeToStr(nbMaxUsers_);
	if (!options.empty())
		options = "+" + options;
	return options;
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
	std::vector<int>::iterator it = findUser(socketArray, socketUser);;
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

/**
 * @brief Checks if the channel has reached the maximum allowed number of users.
 *
 * @return true if the number of connected users equals the maximum limit, false otherwise.
 */
bool Channel::isFull() const { return (nbMaxUsers_ != 0 && socketConnectedUsers_.size() >= nbMaxUsers_); }

/**
 * @brief Checks if the channel is invite-only.
 *
 * @return True if the channel has invite-only restriction enabled, false otherwise.
 */
bool Channel::isInvited(const int socketNewUser) const { return socketInvitedUsers_.find(socketNewUser) != socketInvitedUsers_.end(); }

/**
 * @brief Checks if the channel is invite-only.
 *
 * @return True if the channel has invite-only restriction enabled, false otherwise.
 */
bool Channel::isInviteRestricted() const { return inviteRestricted_; }

/**
 * @brief Checks if the channel's topic can only be changed by operators.
 *
 * @return True if topic changes are restricted to operators, false otherwise.
 */
bool Channel::isTopicRestricted() const { return topicRestricted_; }