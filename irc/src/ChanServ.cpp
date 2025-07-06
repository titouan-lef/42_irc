#include "ChanServ.hpp"
#include "UserManager.hpp"
#include "NoSuchChannelException.hpp"

ChanServ::ChanServ() {}

/**
 * @brief Returns the singleton instance of ChanServ.
 *
 * This method ensures that only one instance of ChanServ exists
 * throughout the program lifetime.
 *
 * @return Reference to the single ChanServ instance.
 */
ChanServ& ChanServ::getInstance()
{
	static ChanServ instance;
	return instance;
}

/**
 * @brief Finds a channel by name or creates a new one if not found.
 *
 * @param name The name of the channel to find.
 * @param user The user requesting the channel creation if not found.
 *
 * @return Reference to the existing or newly created channel.
 */
Channel& ChanServ::findOrCreateChannel(std::string name, const int socketUser)
{
	for (std::vector<Channel>::iterator it = channels_.begin(); it < channels_.end(); ++it)
	{
		if (it->getName() == name)
			return *it;
	}
	return builtNewChannel(name, socketUser);
}

Channel& ChanServ::findChannel(std::string name, const int socketUser)
{
	for (std::vector<Channel>::iterator it = channels_.begin(); it < channels_.end(); ++it)
	{
		if (it->getName() == name)
			return *it;
	}
	throw NoSuchChannelException(socketUser, name);
}

void ChanServ::namesAllChannels(const int socketUser)
{
	for (std::vector<Channel>::iterator it = channels_.begin(); it < channels_.end(); ++it)
	{
		it->sendActualUsers(socketUser);
	}
}

void ChanServ::listAllChannels(const int socketUser)
{
	std::vector<Channel>::iterator it = channels_.begin();
	for (;it < channels_.end(); ++it)
	{
		it->sendChannelSummary(socketUser);
	}
	if (it == channels_.begin())
		return ;
	std::vector<std::string> arg;
 
	arg.push_back(":End of LIST");
	MsgServ::Command cmd = {"323", "", arg};
	MsgServ::getInstance().sendNumericReply(socketUser, cmd, UserManager::getInstance().getUserNickname(socketUser));
}

/**
 * @brief Quit all channel where the user was connected.
 *
 * @param msg The message to send of all member of the channel to find.
 * @param socket The socket of the user quit the server.
 *
 */
void ChanServ::quitAllChannels(const int socket, const std::string& msg)
{
	for (int i = channels_.size() - 1; i >= 0; --i)
	{
		if (channels_.at(i).isConnected(socket))
			logOut(i, msg, socket);
	}
}

void ChanServ::logOut(const std::string &channelName, const std::string &msg, const int socket)
{
	for (int i = channels_.size() - 1; i >= 0; --i)
	{
		if (channels_.at(i).getName() == channelName)
		{
			logOut(i, msg, socket);
			return;
		}
	}
	throw NoSuchChannelException(socket, channelName);
}

/**
 * @brief Creates a new channel with the given name and initial user.
 *
 * @param name The name of the new channel.
 * @param user The user who creates the channel.
 *
 * @return Reference to the newly created channel stored in the channels list.
 */
Channel& ChanServ::builtNewChannel(std::string name, const int socketModerator)
{
	Channel newChannel(name, socketModerator);
	channels_.push_back(newChannel);
	return channels_.back();
}

void ChanServ::deleteChannel(const std::string& name)
{
	for (std::vector<Channel>::iterator it = channels_.begin(); it != channels_.end(); ++it)
	{
		if (it->getName() == name)
		{
			channels_.erase(it);
			break;
		}
	}
}

void ChanServ::deleteChannel(const int index)
{
	channels_.erase(channels_.begin() + index);
}

void ChanServ::logOut(const size_t index, const std::string& msg, const int socket)
{
	channels_[index].sendMessageToAllUsers(socket, msg);
	if(channels_[index].disconnectUser(socket) == -1)
		deleteChannel(index);
	/** @todo send all message for  */
}
