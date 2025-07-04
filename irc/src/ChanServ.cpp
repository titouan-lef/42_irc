#include "ChanServ.hpp"

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
Channel& ChanServ::findChannelFor(std::string name, const int socketUser)
{
	for (std::vector<Channel>::iterator it = channels_.begin(); it < channels_.end(); ++it)
	{
		if (it->getName() == name)
			return *it;
	}
	return builtNewChannel(name, socketUser);
}

/**
 * @brief Quit all channel where the user was connected.
 *
 * @param msg The message to send of all member of the channel to find.
 * @param socket The socket of the user quit the server.
 *
 */
void ChanServ::quitAllChannels(const std::string& msg, const int socket)
{
	for (size_t i = 0; i < channels_.size(); i++)
	{
		if (channels_[i].isConnected(socket))
			logOut(i, msg, socket);
	}
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

void ChanServ::logOut(const size_t index, const std::string& msg, const int socket)
{
	channels_[index].sendMessageToAllUsers(socket, msg);
	channels_[index].disconnectUser(socket);
	/** @todo send all message for  */
}
