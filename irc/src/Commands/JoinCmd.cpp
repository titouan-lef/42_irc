#include "JoinCmd.hpp"

#include "command.hpp"
#include "ChanServ.hpp"
#include "BadChanMaskException.hpp"

#define MAX_ARGS 2
#define MIN_ARGS 1

void JoinCmd::execute(const Command& cmd, const int senderSocket) const
{
	size_t nbArgs = getNbArgs(cmd);
	std::string lastParam = getLastParam(cmd, MIN_ARGS, MAX_ARGS);
	std::vector<std::string> channels;
	std::vector<std::string> passwords;
	if (nbArgs == 1)
	{
		if (lastParam == "0")
		{
			ChanServ::getInstance().partAllChannels(senderSocket);
			return ;
		}
		channels = split(lastParam, ',');
	}
	else
	{
		channels = split(cmd.arg.at(0), ',');
		passwords = split(lastParam, ',');
	}

	size_t j = 0;
	for (size_t i = 0; i < channels.size(); ++i)
	{
		const std::string* pwd = NULL;
		if (isPasswordProtected(channels.at(i)) && j < passwords.size())
		{
			pwd = &passwords.at(j);
			j++;
		}
		try
		{
			tryConnection(channels.at(i), senderSocket, pwd);
		}
		catch (const IrcException& e)
		{
			e.sendError(senderSocket);
		}
	}
}

void JoinCmd::tryConnection(const std::string& channel, const int senderSocket, const std::string* password) const
{
	if (!ChanServ::getInstance().isValidChannelName(channel))
		throw BadChanMaskException(channel);

	Channel& chan = ChanServ::getInstance().findOrCreateChannel(channel, senderSocket);
	
	if (chan.isPasswordProtected())
	{
		if (password)
			chan.connectUser(senderSocket, *password);
		else
			chan.connectUser(senderSocket, "");
	}
	else if (!chan.isConnected(senderSocket))
	{
		chan.connectUser(senderSocket);
	}
}

bool JoinCmd::isPasswordProtected(const std::string &channelName) const
{
	try
	{
		return ChanServ::getInstance().findChannel(channelName).isPasswordProtected();
	}
	catch (...) {}
	return false;
}
