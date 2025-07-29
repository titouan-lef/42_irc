#include "JoinCmd.hpp"

#include "command.hpp"
#include "ChanServ.hpp"
#include "BadChanMaskException.hpp"

JoinCmd::JoinCmd(): ACommand(1, 2) {}

void JoinCmd::execute(const Command& cmd, const int senderSocket) const
{
	checkArgSize(cmd);

	if (isPartAllCmd(cmd.arg))
	{
		ChanServ::getInstance().partAllChannels(senderSocket);
		return;
	}

	std::vector<std::string> chanNameList;
	std::vector<std::string> passwordList;

	if (cmd.arg.size() == nbArgMin_)
		chanNameList = splitTargets(cmd.arg.at(0));
	else
	{
		chanNameList = splitTargets(cmd.arg.at(0));
		passwordList = splitTargets(cmd.arg.at(1));
	}

	std::size_t indexPassword = 0;
	for (std::size_t i = 0; i < chanNameList.size(); ++i)
	{
		const std::string* pwd = NULL;
		const std::string& chanName = chanNameList.at(i);
		if (isPasswordProtected(chanName) && indexPassword < passwordList.size())
		{
			pwd = &passwordList.at(indexPassword);
			++indexPassword;
		}
		try
		{
			tryConnection(chanName, senderSocket, pwd);
		}
		catch (const IrcException& e)
		{
			e.sendError(senderSocket);
		}
	}
}

bool JoinCmd::isPartAllCmd(const std::vector<std::string>& arg) const
{
	return (arg.size() == nbArgMin_ && arg.at(0) == "0");
}

void JoinCmd::tryConnection(const std::string &channel, const int senderSocket, const std::string *password) const
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
		chan.connectUser(senderSocket);
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
