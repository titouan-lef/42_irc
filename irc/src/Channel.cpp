#include "Channel.hpp"
#include "NoSuchNickException.hpp"
#include "PasswordMismatchException.hpp"
#include "ChannelIsFullException.hpp"
#include "ChannelPrivilegesNeeded.hpp"
#include "InviteOnlyException.hpp"
#include "InvalidModeParamException.hpp"

Channel::Channel(std::string name, const int socketModerator): name_(name), nbMaxUsers_(-1), inviteRestricted_(false), topicRestricted_(false)
{
	addNewModerator(socketModerator);
	connectUser(socketModerator);
}

/**
 * @brief Get the name of the channel.
 *
 * @return A constant reference to the channel's name.
 */
const std::string& Channel::getName() const
{
	return name_;
}

/**
 * @brief Get the topic of the channel.
 *
 * @return A constant reference to the channel's topic.
 */
const std::string& Channel::getTopic() const
{
	return topic_;
}

/**
 * @brief Checks if the channel is invite-only.
 *
 * @return True if the channel has invite-only restriction enabled, false otherwise.
 */
bool Channel::isInviteRestricted() const
{
	return inviteRestricted_;
}

/**
 * @brief Checks if the channel's topic can only be changed by moderators.
 *
 * @return True if topic changes are restricted to operators, false otherwise.
 */
bool Channel::isTopicRestricted() const 
{
	return topicRestricted_;
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
	if (isPasswordProtected() && password != password_)
		throw PasswordMismatchException(socketNewUser);/** @todo change with bad_key */
	if (inviteRestricted_ && socketInvitedUsers_.find(socketNewUser) == socketInvitedUsers_.end())
		throw InviteOnlyException(socketNewUser, name_);
	if (isLimitNbUserReached())
		throw ChannelIsFullException(socketNewUser, name_);
	if (inviteRestricted_)
		socketInvitedUsers_.erase(socketNewUser);
	socketConnectedUsers_.push_back(socketNewUser);
}

/**
 * @brief Removes a user from the list of connected users by nickname.
 *
 * @param nickname The nickname of the user to disconnect.
 *
 * @throws NoSuchNickException If no user with the given nickname is found in the channel.
 */
void Channel::disconnectUser(const int socketUser)
{
	for (std::vector<int>::iterator it = socketConnectedUsers_.begin(); it < socketConnectedUsers_.end(); ++it)
	{
		if (*it == socketUser)
		{
			socketConnectedUsers_.erase(it);
			return;
		}
	}
	/** @todo throw NoSuchNickException(nickname); --> define exception in command and call a function to do test in command */
}


/**
 * @brief Checks if a user is a channel moderator by nickname.
 *
 * @param nickname The nickname to check.
 *
 * @return True if the user is a moderator, false otherwise.
 */
bool Channel::isConnected(const int socketUser) const
{
	for (std::vector<int>::const_iterator it = socketConnectedUsers_.begin(); it < socketConnectedUsers_.end(); ++it)
	{
		if (*it == socketUser)
			return true;
	}
	return false;
}

/**
 * @brief Checks if a user is a channel moderator by nickname.
 *
 * @param nickname The nickname to check.
 *
 * @return True if the user is a moderator, false otherwise.
 */
bool Channel::isModerator(const int socketUser) const
{
	for (std::vector<int>::const_iterator it = socketModerators_.begin(); it < socketModerators_.end(); ++it)
	{
		if (*it == socketUser)
			return true;
	}
	return false;
}

/**
 * @brief Restrict topic changes to channel moderators only.
 *
 * @note This function should be called by the MODE command.
 *	   Ensure the user has moderator privileges before applying this mode.
 */
void Channel::enableTopicRestriction()
{
	topicRestricted_ = true;
}

/**
 * @brief Allow all users to change the channel topic.
 *
 * @note This function should be called by the MODE command.
 *	   Ensure the user has moderator privileges before applying this mode.
 */
void Channel::disableTopicRestriction()
{
	topicRestricted_ = false;
}

/**
 * @brief Sets the channel's topic to a new value.
 *
 * @param newTopic The new topic string to assign to the channel.
 * @param user The user trying to change the topic.
 * 
 * @throws ChannelPrivilegesNeeded is the topic is restricted to moderators
 *		 and that the given user is not moderateur.
 */
void Channel::setTopic(const std::string& newTopic, const int socketUser)
{
	if (isTopicRestricted() && !isModerator(socketUser))
		throw ChannelPrivilegesNeeded(socketUser, name_);
	topic_ = newTopic;
}

/**
 * @brief Check if a password is needed to access the channel.
 *
 * @return true if the channel is password protected, false otherwise.
 */
bool Channel::isPasswordProtected() const
{
	return !password_.empty();
}

/**
 * @brief Sets a new password for the channel.
 *
 * @param newPassword The new password to protect the channel.
 */
void  Channel::setPassword(const std::string& newPassword)
{
	password_ = newPassword;
}

/**
 * @brief Disables the channel password.
 */
void Channel::disablePassword()
{
	password_.clear();
}

/**
 * @brief Restrict connection to invited users only.
 *
 * @note This function should be called by the MODE command.
 *	   Ensure the user has moderator privileges before applying this mode.
 */
void Channel::enableInviteRestriction()
{
	inviteRestricted_ = true;
}

/**
 * @brief Allow all users to join the channel.
 *
 * @note This function should be called by the MODE command.
 *	   Ensure the user has moderator privileges before applying this mode.
 */
void Channel::disableInviteRestriction()
{
	inviteRestricted_ = false;
}

void Channel::inviteUser(int socketUser)
{
	socketInvitedUsers_.insert(socketUser); //ERR_USERONCHANNEL
}


/**
 * @brief Adds a new moderator to the channel.
 *
 * @param newModerator Reference to the user to be added as moderator.
 *
 * @note Currently does not check if the user is already a moderator.
 */
void Channel::addNewModerator(const int socketNewModerator)
{
	socketModerators_.push_back(socketNewModerator);
}

/**
 * @brief Removes a moderator from the channel by nickname.
 *
 * @param nickname The nickname of the moderator to remove.
 *
 * @throws NoSuchNickException if no moderator with the given nickname is found.
 */
void Channel::removeModerator(const int socketUser)
{
	for (std::vector<int>::iterator it = socketModerators_.begin(); it < socketModerators_.end(); ++it)
	{
		if (*it == socketUser)
		{
			socketModerators_.erase(it);
			return;
		}
	}
	/** @todo throw NoSuchNickException(nickname);--> define exception in command and call a function to do test in command */
}

/**
 * @brief Checks if the channel has reached the maximum allowed number of users.
 *
 * @return true if the number of connected users equals the maximum limit, false otherwise.
 */
bool Channel::isLimitNbUserReached() const
{
	return static_cast<int>(socketConnectedUsers_.size()) == nbMaxUsers_;
}

/**
 * @brief Sets the maximum number of users allowed in the channel.
 *
 * @param nbUserMax The maximum user count to set.
 * 
 * @return 1 if the given nbUserMax is below 1, 0 otherwise.
 * 
 * @warning you need to throw InvalidModeParamException if it returns 1.
 */
int Channel::setLimitNbUsers(int nbUserMax)
{
	if (nbUserMax <= 0)
		return 1;
	nbMaxUsers_ = nbUserMax;
	return 0;
}

/**
 * @brief Disables the user limit on the channel.
 *
 * Sets the maximum user count to -1, meaning no limit.
 */
void Channel::disableLimitNbUsers()
{
	nbMaxUsers_ = -1;
}

/**
 * @brief Sends a message to all users connected to the channel.
 *
 * @param cmd The command structure with name, trailing and agrs.
 */
void Channel::sendMessageToAllUsers(const int senderSocket, const std::string msg)
{
	MsgServ::Command cmd;
	for (size_t i = 0; i < socketConnectedUsers_.size(); i++)
	{
		if (senderSocket == socketConnectedUsers_[i])
			continue ;
		cmd.trailing = msg;
		MsgServ::sendMessageReply(senderSocket, cmd);
	}
}

/**
 * @brief Sends a numeric to all users connected to the channel.
 *
 * @param cmd The command structure with name, trailing and agrs.
 */
void Channel::sendNumericToAllUsers(const MsgServ::Command& cmd)
{
	(void) cmd;
	/*for (std::vector<User>::iterator it = connectedUser_.begin(); it < connectedUser_.end(); ++it)
	{
		MsgServ::sendNumericReply(it->getSocket(), cmd, it->getNickname());
	}*/
}