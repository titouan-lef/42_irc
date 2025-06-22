#include "Channel.hpp"
#include "NoSuchNickException.hpp"
#include "MsgServ.hpp"

Channel::Channel(std::string name, User& moderator): name_(name)
{
	nbMaxUsers_ = -1;
	inviteRestricted_ = false;
	topicRestricted_ = false;
	addNewModerator(moderator);
	connectUser(moderator);
}

const std::string& Channel::getName()
{
	return name_;
}

bool Channel::isInviteRestricted() const
{
	return inviteRestricted_;
}

bool Channel::isTopicRestricted() const 
{
	return topicRestricted_;
}

void Channel::connectUser(User& newUser)
{
	connectedUser_.push_back(newUser); //check if allreadty present ??
}

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

bool Channel::isModerator(const std::string& nickname) const
{
	for (std::vector<User>::const_iterator it = moderators_.begin(); it < connectedUser_.end(); ++it)
	{
		if (it->getNickname() == nickname)
			return true;
	}
	return false;
}

void Channel::setTopic(const std::string& newTopic)
{
	topic_ = newTopic;
}

void  Channel::setPassword(const std::string& newPassword)
{
	password_ = newPassword;
}

void Channel::disablePassword()
{
	password_.clear();
}

void Channel::addNewModerator(User& newModerator)
{
	moderators_.push_back(newModerator); //check if allreadty present before ??
}

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

void Channel::setLimitUsers(int nbUserMax)
{
	nbMaxUsers_ = nbUserMax;
}

void Channel::disableLimitUsers()
{
	nbMaxUsers_ = -1;
}

void Channel::sendMsgToAllUsers(std::string& msg)
{
	for (std::vector<User>::iterator it = connectedUser_.begin(); it < connectedUser_.end(); ++it)
	{
		MsgServ::sendMsg(it->getSocket(), msg);
	}
}
