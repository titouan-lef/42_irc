#include "JoinCmd.hpp"
#include "UserManager.hpp"
#include "ChanServ.hpp"
#include "NeedMoreParamsException.hpp"
#include "BadChanMaskException.hpp"

void JoinCmd::execute(const Command& cmd, const int senderSocket) const
{
	if (cmd.arg.empty() || cmd.arg.size() > 2 || !cmd.trailing.empty())
		throw NeedMoreParamsException(cmd.name);
	if (cmd.arg.size() == 1 && cmd.arg.at(0) == "0")
	{
		ChanServ::getInstance().quitAllChannels(senderSocket);
		return ;
	}
	std::vector<std::string> channels = split(cmd.arg.at(0), ',');
	std::vector<std::string> passwords;
	if (cmd.arg.size() == 2)
		passwords = split(cmd.arg.at(1), ',');

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
