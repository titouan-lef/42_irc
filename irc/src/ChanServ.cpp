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
ChanServ &ChanServ::getInstance()
{
	static ChanServ instance;
	return instance;
}

void ChanServ::receiveCommand(const int senderSocket, const std::string &cmd)
{
	if (cmd == "help")
		sendHelpNotice(senderSocket);
	else if (cmd == "help JOIN")
		sendHelpJoinNotice(senderSocket);
	else if (cmd == "help PART")
		sendHelpPartNotice(senderSocket);
	else if (cmd == "help TOPIC")
		sendHelpTopicNotice(senderSocket);
	else if (cmd == "help PRIVMSG")
		sendHelpPrivmsgNotice(senderSocket);
	else if (cmd == "help NAMES")
		sendHelpNamesNotice(senderSocket);
	else if (cmd == "help LIST")
		sendHelpListNotice(senderSocket);
	else if (cmd == "help MODE")
		sendHelpModeNotice(senderSocket);
	else if (cmd == "help INVITE")
		sendHelpInviteNotice(senderSocket);
	else if (cmd == "help KICK")
		sendHelpKickNotice(senderSocket);
	else
		MsgServ::getInstance().sendNoticeReply("ChanServ", senderSocket, "Unknown help topic. Use \002help\002 to see available commands.");
}

/*
 * @brief Finds a channel by name or creates a new one if not found.
 *
 * @param name The name of the channel to find.
 * @param user The user requesting the channel creation if not found.
 *
 * @return Reference to the existing or newly created channel.
 */
Channel &ChanServ::findOrCreateChannel(std::string name, const int socketUser)
{
	for (std::vector<Channel>::iterator it = channels_.begin(); it < channels_.end(); ++it)
	{
		if (it->is(name))
			return *it;
	}
	return builtNewChannel(name, socketUser);
}

Channel &ChanServ::findChannel(std::string name)
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
	for (; it < channels_.end(); ++it)
	{
		it->sendChannelSummary(socketUser);
	}
	if (it == channels_.begin())
		return;
	MsgServ::getInstance().sendNumericReply(socketUser, 323, "End of LIST");
}

/**
 * @brief Quit all channel where the user was connected.
 *
 * @param msg The message to send of all member of the channel to find.
 * @param socket The socket of the user quit the server.
 *
 */
void ChanServ::partAllChannels(const int loggedOutSocket, const std::string &msg)
{
	for (int i = channels_.size() - 1; i >= 0; --i)
	{
		if (channels_.at(i).isConnected(loggedOutSocket))
			part(i, loggedOutSocket, msg); /** @todo no channel name in the quit message */
	}
}

void ChanServ::sendBroacastToLinkedUsers(const int socketUser, const std::string &cmdName, const std::vector<std::string> &arg, const std::string &trailing)
{
	std::set<int> socketTargets;

	socketTargets.insert(socketUser);
	for (int i = channels_.size() - 1; i >= 0; --i)
	{
		if (channels_.at(i).isConnected(socketUser))
			channels_.at(i).insertConnectedUsers(socketTargets);
	}
	for (std::set<int>::iterator it = socketTargets.begin(); it != socketTargets.end(); ++it)
	{
		MsgServ::getInstance().sendMessageReply(socketUser, *it, cmdName, arg, trailing);
	}
}

void ChanServ::quit(const int loggedOutSocket, const std::string &msg)
{
	std::set<int> socketTargets;

	for (int i = channels_.size() - 1; i >= 0; --i)
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

void ChanServ::part(const std::string &channelName, const int loggedOutSocket, const std::string &message)
{
	size_t index = findChannelIndex(channelName);
	part(index, loggedOutSocket, message);
}

void ChanServ::kick(const std::string &channelName, const int loggedOutSocket, const int senderSocket, const std::string &message)
{
	size_t index = findChannelIndex(channelName);
	kick(index, loggedOutSocket, senderSocket, message);
}

bool ChanServ::isValidChannelName(const std::string &channelName) const
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
Channel &ChanServ::builtNewChannel(std::string name, const int socketModerator)
{
	Channel newChannel(name, socketModerator);
	channels_.push_back(newChannel);
	return channels_.back();
}

void ChanServ::deleteChannel(const std::string &name)
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
	for (size_t i = 0; i < channels_.size(); ++i)
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

void ChanServ::kick(const size_t index, const int loggedOutSocket, const int senderSocket, const std::string &message)
{
	std::string username = UserManager::getInstance().getUserName(loggedOutSocket);
	channels_.at(index).manageKickMessage(senderSocket, username, message);
	logOut(index, loggedOutSocket);
}

void ChanServ::logOut(const size_t index, const int loggedOutSocket)
{
	Channel &chan = channels_.at(index);
	bool channelEmpty = chan.disconnectUser(loggedOutSocket);
	if (channelEmpty)
		deleteChannel(index);
}

void ChanServ::sendHelpNotice(const int recieverSocket)
{
	MsgServ::getInstance().sendNoticeReply("ChanServ", recieverSocket, "\002***** ChanServ Help *****\002");
	MsgServ::getInstance().sendNoticeReply("ChanServ", recieverSocket, "On this IRC sever, ChanServ is not here to help you manage your channels");
	MsgServ::getInstance().sendNoticeReply("ChanServ", recieverSocket, "but only informs you of on witch command is aviable to you on the server to manipulate channels.");
	MsgServ::getInstance().sendNoticeReply("ChanServ", recieverSocket, "You can use the following command:");
	MsgServ::getInstance().sendNoticeReply("ChanServ", recieverSocket, "\002JOIN\002            Join or create a channel");
	MsgServ::getInstance().sendNoticeReply("ChanServ", recieverSocket, "\002PART\002            Leave the channel");
	MsgServ::getInstance().sendNoticeReply("ChanServ", recieverSocket, "\002TOPIC\002           Change or show the channel's topic");
	MsgServ::getInstance().sendNoticeReply("ChanServ", recieverSocket, "\002PRIVMSG\002         Send a message on the channel");
	MsgServ::getInstance().sendNoticeReply("ChanServ", recieverSocket, "\002NAMES\002           List the channels and their users");
	MsgServ::getInstance().sendNoticeReply("ChanServ", recieverSocket, "\002LIST\002            List the channels");
	MsgServ::getInstance().sendNoticeReply("ChanServ", recieverSocket, "If you are operator of a chanel, you can aslo use:");
	MsgServ::getInstance().sendNoticeReply("ChanServ", recieverSocket, "\002MODE\002            Moderate a channel");
	MsgServ::getInstance().sendNoticeReply("ChanServ", recieverSocket, "\002INVITE\002          Invite someone to join a channel");
	MsgServ::getInstance().sendNoticeReply("ChanServ", recieverSocket, "\002KICK\002            Kick someone from a channel");
	MsgServ::getInstance().sendNoticeReply("ChanServ", recieverSocket, "For more details on a command, type \"PRIVMSG ChanServ :help <cmd>\" or \"/msg ChanServ help <cmd>\".");
	MsgServ::getInstance().sendNoticeReply("ChanServ", recieverSocket, "\002***** End of Help *****\002");
}

void ChanServ::sendHelpJoinNotice(const int recieverSocket)
{
	MsgServ::getInstance().sendNoticeReply("ChanServ", recieverSocket, "\002***** ChanServ Help *****\002");
	MsgServ::getInstance().sendNoticeReply("ChanServ", recieverSocket, "The JOIN command allows to join a channel or create it if it does not exist.");
	MsgServ::getInstance().sendNoticeReply("ChanServ", recieverSocket, "\002Format: JOIN <channels> [passwords]\002");
	MsgServ::getInstance().sendNoticeReply("ChanServ", recieverSocket, "\002<channels>\002      The names of the channel you want to join separeted with a ','");
	MsgServ::getInstance().sendNoticeReply("ChanServ", recieverSocket, "\002<passwords>\002     (Optional) The password needed by channels separeted with a ','");
	MsgServ::getInstance().sendNoticeReply("ChanServ", recieverSocket, "You can use \"JOIN 0\" to leave all the channel you are in.");
	MsgServ::getInstance().sendNoticeReply("ChanServ", recieverSocket, "\002***** End of Help *****\002");
}

void ChanServ::sendHelpPartNotice(const int receiverSocket)
{
	MsgServ::getInstance().sendNoticeReply("ChanServ", receiverSocket, "\002***** ChanServ Help *****\002");
	MsgServ::getInstance().sendNoticeReply("ChanServ", receiverSocket, "The PART command lets you leave one or severals channels.");
	MsgServ::getInstance().sendNoticeReply("ChanServ", receiverSocket, "You can provide a reason to you leaving.");
	MsgServ::getInstance().sendNoticeReply("ChanServ", receiverSocket, "\002Format: PART <channels> [reason]\002");
	MsgServ::getInstance().sendNoticeReply("ChanServ", receiverSocket, "\002<channels>\002      Names of channels to leave, separated by ','");
	MsgServ::getInstance().sendNoticeReply("ChanServ", receiverSocket, "\002<reason>\002        (Optional) Reason for leaving");
	MsgServ::getInstance().sendNoticeReply("ChanServ", receiverSocket, "\002***** End of Help *****\002");
}

void ChanServ::sendHelpTopicNotice(const int receiverSocket)
{
	MsgServ::getInstance().sendNoticeReply("ChanServ", receiverSocket, "\002***** ChanServ Help *****\002");
	MsgServ::getInstance().sendNoticeReply("ChanServ", receiverSocket, "The TOPIC command changes or shows the topic of a channel.");
	MsgServ::getInstance().sendNoticeReply("ChanServ", receiverSocket, "Changing the popic of a channel can be restricted to operators.");
	MsgServ::getInstance().sendNoticeReply("ChanServ", receiverSocket, "In that case if you are not operator of the channel, the c ommand will fail.");
	MsgServ::getInstance().sendNoticeReply("ChanServ", receiverSocket, "\002Format: TOPIC <channel> [topic]\002");
	MsgServ::getInstance().sendNoticeReply("ChanServ", receiverSocket, "\002<channel>\002      The channel name");
	MsgServ::getInstance().sendNoticeReply("ChanServ", receiverSocket, "\002<topic>\002        (Optional) The new topic");
	MsgServ::getInstance().sendNoticeReply("ChanServ", receiverSocket, "If you dont provide a topic, the actual topic will be send to you.");
	MsgServ::getInstance().sendNoticeReply("ChanServ", receiverSocket, "\002***** End of Help *****\002");
}

void ChanServ::sendHelpPrivmsgNotice(const int receiverSocket)
{
	MsgServ::getInstance().sendNoticeReply("ChanServ", receiverSocket, "\002***** ChanServ Help *****\002");
	MsgServ::getInstance().sendNoticeReply("ChanServ", receiverSocket, "The PRIVMSG command sends a private message to a user or channel.");
	MsgServ::getInstance().sendNoticeReply("ChanServ", receiverSocket, "\002Format: PRIVMSG <target> :<message>\002");
	MsgServ::getInstance().sendNoticeReply("ChanServ", receiverSocket, "\002<target>\002       Nickname or channel name");
	MsgServ::getInstance().sendNoticeReply("ChanServ", receiverSocket, "\002<message>\002      The message to send");
	MsgServ::getInstance().sendNoticeReply("ChanServ", receiverSocket, "\002***** End of Help *****\002");
}

void ChanServ::sendHelpNamesNotice(const int receiverSocket)
{
	MsgServ::getInstance().sendNoticeReply("ChanServ", receiverSocket, "\002***** ChanServ Help *****\002");
	MsgServ::getInstance().sendNoticeReply("ChanServ", receiverSocket, "The NAMES command shows the nicknames of users on a channel.");
	MsgServ::getInstance().sendNoticeReply("ChanServ", receiverSocket, "\002Format: NAMES [channels]\002");
	MsgServ::getInstance().sendNoticeReply("ChanServ", receiverSocket, "\002<channels>\002      (Optional) Names of channels separated by ','");
	MsgServ::getInstance().sendNoticeReply("ChanServ", receiverSocket, "If you dont provide the name(s) of the channel(s) it will list all the channle on the server.");
	MsgServ::getInstance().sendNoticeReply("ChanServ", receiverSocket, "\002***** End of Help *****\002");
}

void ChanServ::sendHelpListNotice(const int receiverSocket)
{
	MsgServ::getInstance().sendNoticeReply("ChanServ", receiverSocket, "\002***** ChanServ Help *****\002");
	MsgServ::getInstance().sendNoticeReply("ChanServ", receiverSocket, "The LIST command shows channels and their topics.");
	MsgServ::getInstance().sendNoticeReply("ChanServ", receiverSocket, "\002Format: LIST [channels]\002");
	MsgServ::getInstance().sendNoticeReply("ChanServ", receiverSocket, "\002<channels>\002      (Optional) names of channels to list");
	MsgServ::getInstance().sendNoticeReply("ChanServ", receiverSocket, "If you dont provide the name(s) of the channel(s) it will list all the channle on the server.");
	MsgServ::getInstance().sendNoticeReply("ChanServ", receiverSocket, "\002***** End of Help *****\002");
}

void ChanServ::sendHelpModeNotice(const int receiverSocket)
{
	MsgServ::getInstance().sendNoticeReply("ChanServ", receiverSocket, "\002***** ChanServ Help *****\002");
	MsgServ::getInstance().sendNoticeReply("ChanServ", receiverSocket, "The MODE command changes or shows a channel or user mode.");
	MsgServ::getInstance().sendNoticeReply("ChanServ", receiverSocket, "\002Format: MODE <target> [(+/-<modes> [params]]\002");
	MsgServ::getInstance().sendNoticeReply("ChanServ", receiverSocket, "\002<target>\002        Names of channels separated by ','");
	MsgServ::getInstance().sendNoticeReply("ChanServ", receiverSocket, "\002<modes>\002         Mode string to change");
	MsgServ::getInstance().sendNoticeReply("ChanServ", receiverSocket, "\002<params>\002        (Opional) Mode parameters");
	MsgServ::getInstance().sendNoticeReply("ChanServ", receiverSocket, "The following modes are available:");
	MsgServ::getInstance().sendNoticeReply("ChanServ", receiverSocket, "\002i\002               Set/remove Invite-only channel");
	MsgServ::getInstance().sendNoticeReply("ChanServ", receiverSocket, "\002t\002               Set/remove the restrictions of the TOPIC command to channel operators");
	MsgServ::getInstance().sendNoticeReply("ChanServ", receiverSocket, "\002k\002               Set/remove the channel key (password)");
	MsgServ::getInstance().sendNoticeReply("ChanServ", receiverSocket, "\002o\002               Give/take channel operator privilege");
	MsgServ::getInstance().sendNoticeReply("ChanServ", receiverSocket, "\002l\002               Set/remove the user limit to channel");
	MsgServ::getInstance().sendNoticeReply("ChanServ", receiverSocket, "If you dont provide any modes it will send the actual channel modes.");
	MsgServ::getInstance().sendNoticeReply("ChanServ", receiverSocket, "\002***** End of Help *****\002");
}

void ChanServ::sendHelpInviteNotice(const int receiverSocket)
{
	MsgServ::getInstance().sendNoticeReply("ChanServ", receiverSocket, "\002***** ChanServ Help *****\002");
	MsgServ::getInstance().sendNoticeReply("ChanServ", receiverSocket, "The INVITE command invites a user to a channel.");
	MsgServ::getInstance().sendNoticeReply("ChanServ", receiverSocket, "\002Format: INVITE <nick> <channel>\002");
	MsgServ::getInstance().sendNoticeReply("ChanServ", receiverSocket, "\002<nick>\002         Nickname to invite.");
	MsgServ::getInstance().sendNoticeReply("ChanServ", receiverSocket, "\002<channel>\002      Channel name.");
	MsgServ::getInstance().sendNoticeReply("ChanServ", receiverSocket, "\002***** End of Help *****\002");
}

void ChanServ::sendHelpKickNotice(const int receiverSocket)
{
	MsgServ::getInstance().sendNoticeReply("ChanServ", receiverSocket, "\002***** ChanServ Help *****\002");
	MsgServ::getInstance().sendNoticeReply("ChanServ", receiverSocket, "The KICK command removes a user from a channel.");
	MsgServ::getInstance().sendNoticeReply("ChanServ", receiverSocket, "\002Format: KICK <channel> <user> [reason]\002");
	MsgServ::getInstance().sendNoticeReply("ChanServ", receiverSocket, "\002<channel>\002      Names of channels separated by ','.");
	MsgServ::getInstance().sendNoticeReply("ChanServ", receiverSocket, "\002<user>\002         Names of users to kick separated by ','.");
	MsgServ::getInstance().sendNoticeReply("ChanServ", receiverSocket, "\002<reason>\002       (Optional) Reason for the kick.");
	MsgServ::getInstance().sendNoticeReply("ChanServ", receiverSocket, "You can eather provite one channel and severals users");
	MsgServ::getInstance().sendNoticeReply("ChanServ", receiverSocket, "or as many channels that users.");
	MsgServ::getInstance().sendNoticeReply("ChanServ", receiverSocket, "\002***** End of Help *****\002");
}
