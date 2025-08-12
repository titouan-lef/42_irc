#include "ChanServ.hpp"

#include "MsgServ.hpp"
#include "UserManager.hpp"
#include "NoSuchChannelException.hpp"
#include "command.hpp"

ChanServ::ChanServ(): Service("ChanServ")
{
	helpNoticeMap_["help"] = &ChanServ::sendHelpNotice;
	helpNoticeMap_["help JOIN"] = &ChanServ::sendHelpJoinNotice;
	helpNoticeMap_["help PART"] = &ChanServ::sendHelpPartNotice;
	helpNoticeMap_["help TOPIC"] = &ChanServ::sendHelpTopicNotice;
	helpNoticeMap_["help PRIVMSG"] = &ChanServ::sendHelpPrivmsgNotice;
	helpNoticeMap_["help NAMES"] = &ChanServ::sendHelpNamesNotice;
	helpNoticeMap_["help LIST"] = &ChanServ::sendHelpListNotice;
	helpNoticeMap_["help MODE"] = &ChanServ::sendHelpModeNotice;
	helpNoticeMap_["help INVITE"] = &ChanServ::sendHelpInviteNotice;
	helpNoticeMap_["help KICK"] = &ChanServ::sendHelpKickNotice;
}

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

void ChanServ::receiveCommand(const int senderSocket, const std::string& cmd)
{
	const std::string DEFAULT_REPLY = "Unknown help topic. Use \002help\002 to see available commands.";

	this_func_map_t::iterator it = helpNoticeMap_.find(cmd);
	if (it == helpNoticeMap_.end())
		sendNoticeReply(senderSocket, DEFAULT_REPLY);
	else
		(this->*it->second)(senderSocket);
}

/**
 * @brief Creates a new channel with the given name and initial user.
 *
 * @param name The name of the new channel.
 * @param user The user who creates the channel.
 *
 * @return Reference to the newly created channel stored in the channels list.
 */
Channel& ChanServ::createChannelWithOperator(const std::string& name, const int socketOperator)
{
	Channel newChannel(name, socketOperator);
	channelList_.push_back(newChannel);
	return channelList_.back();
}

Channel* ChanServ::searchChannel(const std::string& name)
{
	try
	{
		Channel& chan = findChannel(name);
		return &chan;
	}
	catch(const NoSuchChannelException& e)
	{
		return NULL;
	}
}

Channel& ChanServ::findChannel(const std::string& name)
{
	std::size_t index = findChannelIndex(name);
	return channelList_.at(index);
}

void ChanServ::namesAllChannels(const int socketUser) const
{
	for (std::size_t i = 0; i < channelList_.size(); ++i)
		channelList_.at(i).sendActualUsers(socketUser);
}

void ChanServ::listAllChannels(const int socketUser) const
{
	if (channelList_.empty())
		return;
	for (std::size_t i = 0; i < channelList_.size(); ++i)
		channelList_.at(i).sendChannelSummary(socketUser);
	MsgServ::getInstance().sendNumericReply(socketUser, 323, "End of LIST");
}

/**
 * @brief Quit all channel where the user was connected.
 *
 * @param msg The message to send of all member of the channel to find.
 * @param socket The socket of the user quit the server.
 *
 */
void ChanServ::partAllChannels(const int loggedOutSocket, const char* message)
{
	for (int i = channelList_.size() - 1; i >= 0; --i)
	{
		if (channelList_.at(i).isConnected(loggedOutSocket))
			part(i, loggedOutSocket, message); /** @todo no channel name in the quit message */
	}
}

void ChanServ::sendBroacastToLinkedUsers(const int socketUser, const Command& cmd)
{
	std::set<int> socketTargets;

	socketTargets.insert(socketUser);
	for (int i = channelList_.size() - 1; i >= 0; --i)
	{
		if (channelList_.at(i).isConnected(socketUser))
			channelList_.at(i).insertConnectedUsers(socketTargets);
	}
	for (std::set<int>::iterator it = socketTargets.begin(); it != socketTargets.end(); ++it)
		MsgServ::getInstance().sendMessageReply(socketUser, *it, cmd);
}

void ChanServ::quit(const int loggedOutSocket, const char* message)
{
	std::set<int> socketTargets;

	for (int i = channelList_.size() - 1; i >= 0; --i)
	{
		if (channelList_.at(i).isConnected(loggedOutSocket))
		{
			channelList_.at(i).insertConnectedUsers(socketTargets);
			logOut(i, loggedOutSocket);
		}
	}
	Command cmd;
	cmd.name = "QUIT";
	cmd.hasTrailing = (message != NULL);
	if (cmd.hasTrailing)
		cmd.arg.push_back(message);
	for (std::set<int>::iterator it = socketTargets.begin(); it != socketTargets.end(); ++it)
		MsgServ::getInstance().sendMessageReply(loggedOutSocket, *it, cmd);
}

void ChanServ::part(const std::string& channelName, const int loggedOutSocket, const char* message)
{
	std::size_t index = findChannelIndex(channelName);
	part(index, loggedOutSocket, message);
}

void ChanServ::kick(const std::string& channelName, const int loggedOutSocket, const int senderSocket, const std::string& message)
{
	std::size_t index = findChannelIndex(channelName);
	kick(index, loggedOutSocket, senderSocket, message);
}

bool ChanServ::isValidChannelName(const std::string& channelName) const
{
	return (!channelName.empty() && channelName.size() <= 50 && channelName.find_first_of("#&+!") == 0 &&
		channelName.find_first_of(",\a") == std::string::npos);
}

void ChanServ::deleteChannel(const std::string& name)
{
	try
	{
		std::size_t index = findChannelIndex(name);
		deleteChannel(index);
	}
	catch(const NoSuchChannelException& e) {}
}

std::size_t ChanServ::findChannelIndex(const std::string& channelName) const
{
	for (std::size_t i = 0; i < channelList_.size(); ++i)
	{
		if (channelList_.at(i).is(channelName))
			return i;
	}
	throw NoSuchChannelException(channelName);
}

void ChanServ::deleteChannel(const std::size_t index)
{
	channelList_.erase(channelList_.begin() + index);
}

void ChanServ::part(const std::size_t index, const int loggedOutSocket, const char* message)
{
	channelList_.at(index).managePartMessage(loggedOutSocket, message);
	logOut(index, loggedOutSocket);
}

void ChanServ::kick(const std::size_t index, const int loggedOutSocket, const int senderSocket, const std::string& message)
{
	const std::string username = UserManager::getInstance().findUserName(loggedOutSocket);
	channelList_.at(index).manageKickMessage(senderSocket, username, message);
	logOut(index, loggedOutSocket);
}

void ChanServ::logOut(const std::size_t index, const int loggedOutSocket)
{
	Channel &chan = channelList_.at(index);
	bool channelEmpty = chan.disconnectUser(loggedOutSocket);
	if (channelEmpty)
		deleteChannel(index);
}

void ChanServ::sendHelpNotice(const int receiverSocket) const
{
	const std::size_t nbMessage = 14;
	const std::string tabMessage[nbMessage] = {
		"On this IRC sever, ChanServ is not here to help you manage your channels",
		"but only informs you of on witch command is aviable to you on the server to manipulate channels.",
		"You can use the following command:",
		"\002JOIN\002            Join or create a channel",
		"\002PART\002            Leave the channel",
		"\002TOPIC\002           Change or show the channel's topic",
		"\002PRIVMSG\002         Send a message on the channel",
		"\002NAMES\002           List the channels and their users",
		"\002LIST\002            List the channels",
		"If you are operator of a chanel, you can aslo use:",
		"\002MODE\002            Moderate a channel",
		"\002INVITE\002          Invite someone to join a channel",
		"\002KICK\002            Kick someone from a channel",
		"For more details on a command, type \"PRIVMSG ChanServ :help <cmd>\" or \"/msg ChanServ help <cmd>\"."
	};
	sendHelpNoticeReply(receiverSocket, tabMessage, nbMessage);
}

void ChanServ::sendHelpJoinNotice(const int receiverSocket) const
{
	const std::size_t nbMessage = 5;
	const std::string tabMessage[nbMessage] = {
		"The JOIN command allows to join a channel or create it if it does not exist.",
		"\002Format: JOIN <channels> [passwords]\002",
		"\002<channels>\002      The names of the channel you want to join separeted with a ','",
		"\002<passwords>\002     (Optional) The password needed by channels separeted with a ','",
		"You can use \"JOIN 0\" to leave all the channel you are in."
	};
	sendHelpNoticeReply(receiverSocket, tabMessage, nbMessage);
}

void ChanServ::sendHelpPartNotice(const int receiverSocket) const
{
	const std::size_t nbMessage = 5;
	const std::string tabMessage[nbMessage] = {
		"The PART command lets you leave one or severals channels.",
		"You can provide a reason to you leaving.",
		"\002Format: PART <channels> [reason]\002",
		"\002<channels>\002      Names of channels to leave, separated by ','",
		"\002<reason>\002        (Optional) Reason for leaving"
	};
	sendHelpNoticeReply(receiverSocket, tabMessage, nbMessage);
}

void ChanServ::sendHelpTopicNotice(const int receiverSocket) const
{
	const std::size_t nbMessage = 7;
	const std::string tabMessage[nbMessage] = {
		"The TOPIC command changes or shows the topic of a channel.",
		"Changing the popic of a channel can be restricted to operators.",
		"In that case if you are not operator of the channel, the c ommand will fail.",
		"\002Format: TOPIC <channel> [topic]\002",
		"\002<channel>\002      The channel name",
		"\002<topic>\002        (Optional) The new topic",
		"If you dont provide a topic, the actual topic will be send to you."
	};
	sendHelpNoticeReply(receiverSocket, tabMessage, nbMessage);
}

void ChanServ::sendHelpPrivmsgNotice(const int receiverSocket) const
{
	const std::size_t nbMessage = 4;
	const std::string tabMessage[nbMessage] = {
		"The PRIVMSG command sends a private message to a user or channel.",
		"\002Format: PRIVMSG <target> :<message>\002",
		"\002<target>\002       Nickname or channel name",
		"\002<message>\002      The message to send"
	};
	sendHelpNoticeReply(receiverSocket, tabMessage, nbMessage);
}

void ChanServ::sendHelpNamesNotice(const int receiverSocket) const
{
	const std::size_t nbMessage = 4;
	const std::string tabMessage[nbMessage] = {
		"The NAMES command shows the nicknames of users on a channel.",
		"\002Format: NAMES [channels]\002",
		"\002<channels>\002      (Optional) Names of channels separated by ','",
		"If you dont provide the name(s) of the channel(s) it will list all the channle on the server."
	};
	sendHelpNoticeReply(receiverSocket, tabMessage, nbMessage);
}

void ChanServ::sendHelpListNotice(const int receiverSocket) const
{
	const std::size_t nbMessage = 4;
	const std::string tabMessage[nbMessage] = {
		"The LIST command shows channels and their topics.",
		"\002Format: LIST [channels]\002",
		"\002<channels>\002      (Optional) names of channels to list",
		"If you dont provide the name(s) of the channel(s) it will list all the channle on the server."
	};
	sendHelpNoticeReply(receiverSocket, tabMessage, nbMessage);
}

void ChanServ::sendHelpModeNotice(const int receiverSocket) const
{
	const std::size_t nbMessage = 12;
	const std::string tabMessage[nbMessage] = {
		"The MODE command changes or shows a channel or user mode.",
		"\002Format: MODE <target> [(+/-<modes> [params]]\002",
		"\002<target>\002        Names of channels separated by ','",
		"\002<modes>\002         Mode string to change",
		"\002<params>\002        (Opional) Mode parameters",
		"The following modes are available:",
		"\002i\002               Set/remove Invite-only channel",
		"\002t\002               Set/remove the restrictions of the TOPIC command to channel operators",
		"\002k\002               Set/remove the channel key (password)",
		"\002o\002               Give/take channel operator privilege",
		"\002l\002               Set/remove the user limit to channel",
		"If you dont provide any modes it will send the actual channel modes."
	};
	sendHelpNoticeReply(receiverSocket, tabMessage, nbMessage);
}

void ChanServ::sendHelpInviteNotice(const int receiverSocket) const
{
	const std::size_t nbMessage = 4;
	const std::string tabMessage[nbMessage] = {
		"The INVITE command invites a user to a channel.",
		"\002Format: INVITE <nick> <channel>\002",
		"\002<nick>\002         Nickname to invite.",
		"\002<channel>\002      Channel name."
	};
	sendHelpNoticeReply(receiverSocket, tabMessage, nbMessage);
}

void ChanServ::sendHelpKickNotice(const int receiverSocket) const
{
	const std::size_t nbMessage = 7;
	const std::string tabMessage[nbMessage] = {
		"The KICK command removes a user from a channel.",
		"\002Format: KICK <channel> <user> [reason]\002",
		"\002<channel>\002      Names of channels separated by ','.",
		"\002<user>\002         Names of users to kick separated by ','.",
		"\002<reason>\002       (Optional) Reason for the kick.",
		"You can eather provite one channel and severals users",
		"or as many channels that users.",
	};
	sendHelpNoticeReply(receiverSocket, tabMessage, nbMessage);
}

void ChanServ::sendHelpNoticeReply(const int receiverSocket, const std::string* tabMessage, const std::size_t nbMessage) const
{
	sendNoticeReply(receiverSocket, "\002***** ChanServ Help *****\002");
	for (std::size_t i = 0; i < nbMessage; ++i)
		sendNoticeReply(receiverSocket, tabMessage[i]);
	sendNoticeReply(receiverSocket, "\002***** End of Help *****\002");
}

void ChanServ::sendNoticeReply(const int receiverSocket, const std::string& message) const
{
	MsgServ::getInstance().sendNoticeReply("ChanServ", receiverSocket, message);
}
