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

Channel& ChanServ::findChannel(std::string name)
{
	for (std::vector<Channel>::iterator it = channels_.begin(); it < channels_.end(); ++it)
	{
		if (it->getName() == name)
			return *it;
	}
	throw NoSuchChannelException(name);
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
	MsgServ::getInstance().sendNumericReply(socketUser, 323, "End of LIST");
}

/**
 * @brief Quit all channel where the user was connected.
 *
 * @param msg The message to send of all member of the channel to find.
 * @param socket The socket of the user quit the server.
 *
 */
void ChanServ::quitAllChannels(const int loggedOutSocket, const std::string& msg)
{
	Command cmd;
	cmd.name = "QUIT";
	cmd.trailing = msg;
	for (int i = channels_.size() - 1; i >= 0; --i)
	{
		if (channels_.at(i).isConnected(loggedOutSocket))
			logOut(i, cmd, loggedOutSocket);
	}
}

void ChanServ::logOut(const std::string &channelName, const Command& cmd, const int loggedOutSocket)
{
	for (int i = channels_.size() - 1; i >= 0; --i)
	{
		if (channels_.at(i).getName() == channelName)
		{
			logOut(i, cmd, loggedOutSocket);
			return;
		}
	}
	throw NoSuchChannelException(channelName);
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

void ChanServ::logOut(const size_t index, const Command& cmd, const int loggedOutSocket)
{
	channels_.at(index).sendMessageToAllUsers(loggedOutSocket, cmd);
	if(channels_.at(index).disconnectUser(loggedOutSocket) == -1)
		deleteChannel(index);
	/** @todo send all message for  */
}
