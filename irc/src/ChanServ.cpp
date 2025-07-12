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
		if (it->is(name))
			return *it;
	}
	return builtNewChannel(name, socketUser);
}

Channel& ChanServ::findChannel(std::string name)
{
	for (std::vector<Channel>::iterator it = channels_.begin(); it < channels_.end(); ++it)
	{
		if (it->is(name))
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
void ChanServ::partAllChannels(const int loggedOutSocket, const std::string& msg)
{
	for (int i = channels_.size() - 1; i >= 0; --i)
	{
		if (channels_.at(i).isConnected(loggedOutSocket))
			part(i, loggedOutSocket, msg); /** @todo no channel name in the quit message */
	}
}

void ChanServ::sendBroacastToLinkedUsers(const int socketUser, const Command& cmd)
{
	std::set<int> socketTargets;

	socketTargets.insert(socketUser);
	for (int i = channels_.size()- 1; i >= 0 ; --i)
	{
		if (channels_.at(i).isConnected(socketUser))
			channels_.at(i).insertConnectedUsers(socketTargets);
	}
	for (std::set<int>::iterator it = socketTargets.begin(); it != socketTargets.end(); ++it)
	{
		MsgServ::getInstance().sendMessageReply(socketUser, *it, cmd.name, cmd.arg, cmd.trailing);
	}
}

void ChanServ::quit(const int loggedOutSocket, const std::string& msg)
{
	std::set<int> socketTargets;

	for (int i = channels_.size()- 1; i >= 0 ; --i)
	{
		if (channels_.at(i).isConnected(loggedOutSocket))
		{
			channels_.at(i).insertConnectedUsers(socketTargets);
			logOut(i, loggedOutSocket);
		}
	}
	for (std::set<int>::iterator it = socketTargets.begin(); it != socketTargets.end(); ++it)
	{
		MsgServ::getInstance().sendMessageReply(loggedOutSocket, *it, "QUIT", std::vector<std::string>(), msg);
	}
}

void ChanServ::part(const std::string& channelName, const int loggedOutSocket, const std::string& message)
{ 
	size_t index = findChannelIndex(channelName);
	part(index, loggedOutSocket, message);
}

void ChanServ::kick(const std::string &channelName, const int loggedOutSocket, const int senderSocket, const std::string& message)
{
	size_t index = findChannelIndex(channelName);
	kick(index, loggedOutSocket, senderSocket, message);
}

bool ChanServ::isValidChannelName(const std::string& channelName) const
{
	if (channelName.empty() || channelName.size() > 50)
		return false;
	if (channelName.find_first_of("#&+!") != 0)
		return false;
	if (channelName.find_first_of(",\a") != std::string::npos)
		return false;
	return true;
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
		if (it->is(name))
		{
			channels_.erase(it);
			break;
		}
	}
}

size_t ChanServ::findChannelIndex(std::string channelName)
{
	for (size_t i = 0 ; i < channels_.size(); ++i)
	{
		if (channels_.at(i).is(channelName))
			return i;
	}
	throw NoSuchChannelException(channelName);
}

void ChanServ::deleteChannel(const int index)
{
	channels_.erase(channels_.begin() + index);
}

void ChanServ::part(const size_t index, const int loggedOutSocket, const std::string &message)
{
	channels_.at(index).managePartMessage(loggedOutSocket, message);
	logOut(index, loggedOutSocket);
}

void ChanServ::kick(const size_t index, const int loggedOutSocket, const int senderSocket, const std::string& message)
{
	std::string username = UserManager::getInstance().getUserName(loggedOutSocket);
	channels_.at(index).manageKickMessage(senderSocket, username, message);
	logOut(index, loggedOutSocket);
}

void ChanServ::logOut(const size_t index, const int loggedOutSocket)
{
	Channel& chan = channels_.at(index);
	bool channelEmpty = chan.disconnectUser(loggedOutSocket);
	if (channelEmpty)
		deleteChannel(index);
}