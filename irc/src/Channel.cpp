#include "Channel.hpp"
#include "NoSuchNickException.hpp"
#include "PasswordMismatchException.hpp"
#include "ChannelIsFullException.hpp"
#include "ChannelPrivilegesNeeded.hpp"
#include "MsgServ.hpp"

Channel::Channel(std::string name, User& moderator): name_(name), nbMaxUsers_(-1), inviteRestricted_(false), topicRestricted_(false)
{
	addNewModerator(moderator);
	connectUser(moderator);
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
 * 
 * @throws PasswordMismatchException if the given password is incorect.
 * @throws ChannelIsFullException if the maximum number of users is reached.
 */
void Channel::connectUser(User& newUser, const std::string& password)
{
	if (isPasswordProtected() && password != password_)
		throw PasswordMismatchException();
	if (isLimitNbUserReached())
		throw ChannelIsFullException(name_);
	connectedUser_.push_back(newUser);
}

/**
 * @brief Removes a user from the list of connected users by nickname.
 *
 * @param nickname The nickname of the user to disconnect.
 *
 * @throws NoSuchNickException If no user with the given nickname is found in the channel.
 */
void Channel::disconnectUser(const std::string& nickname)
{
	for (std::vector<User>::iterator it = connectedUser_.begin(); it < connectedUser_.end(); ++it)
	{
		if (it->getNickname() == nickname)
		{
			connectedUser_.erase(it);
			return;
		}
	}
	throw NoSuchNickException(nickname);
}


/**
 * @brief Checks if a user is a channel moderator by nickname.
 *
 * @param nickname The nickname to check.
 *
 * @return True if the user is a moderator, false otherwise.
 */
bool Channel::isConnected(const std::string& nickname) const
{
	for (std::vector<User>::const_iterator it = connectedUser_.begin(); it < connectedUser_.end(); ++it)
	{
		if (it->getNickname() == nickname)
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
bool Channel::isModerator(const std::string& nickname) const
{
	for (std::vector<User>::const_iterator it = moderators_.begin(); it < moderators_.end(); ++it)
	{
		if (it->getNickname() == nickname)
			return true;
	}
	return false;
}

/**
 * @brief Restrict topic changes to channel moderators only.
 *
 * @note This function should be called by the MODE command.
 *       Ensure the user has moderator privileges before applying this mode.
 */
void Channel::enableTopicRestriction()
{
	topicRestricted_ = true;
}

/**
 * @brief Allow all users to change the channel topic.
 *
 * @note This function should be called by the MODE command.
 *       Ensure the user has moderator privileges before applying this mode.
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
 *         and that the given user is not moderateur.
 */
void Channel::setTopic(const std::string& newTopic, const User& user)
{
	if (isTopicRestricted() && !isModerator(user.getNickname()))
		throw ChannelPrivilegesNeeded();
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
 * @brief Adds a new moderator to the channel.
 *
 * @param newModerator Reference to the user to be added as moderator.
 *
 * @note Currently does not check if the user is already a moderator.
 */
void Channel::addNewModerator(User& newModerator)
{
	moderators_.push_back(newModerator);
}

/**
 * @brief Removes a moderator from the channel by nickname.
 *
 * @param nickname The nickname of the moderator to remove.
 *
 * @throws NoSuchNickException if no moderator with the given nickname is found.
 */
void Channel::removeModerator(const std::string& nickname)
{
	for (std::vector<User>::iterator it = moderators_.begin(); it < moderators_.end(); ++it)
	{
		if (it->getNickname() == nickname)
		{
			moderators_.erase(it);
			return;
		}
	}
	throw NoSuchNickException(nickname);
}

/**
 * @brief Checks if the channel has reached the maximum allowed number of users.
 *
 * @return true if the number of connected users equals the maximum limit, false otherwise.
 */
bool Channel::isLimitNbUserReached() const
{
	return static_cast<int>(connectedUser_.size()) == nbMaxUsers_;
}

/**
 * @brief Sets the maximum number of users allowed in the channel.
 *
 * @param nbUserMax The maximum user count to set.
 */
void Channel::setLimitNbUsers(int nbUserMax)
{
	nbMaxUsers_ = nbUserMax;
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
 * @param msg The message string to send.
 */
void Channel::sendMsgToAllUsers(std::string& msg)
{
	for (std::vector<User>::iterator it = connectedUser_.begin(); it < connectedUser_.end(); ++it)
	{
		MsgServ::sendMsg(it->getSocket(), msg);
	}
}
