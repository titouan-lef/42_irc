#include "JoinCmd.hpp"

#include "command.hpp"
#include "ChanServ.hpp"
#include "BadChanMaskException.hpp"

void JoinCmd::execute(const Command& cmd, const int senderSocket) const
{
	const std::size_t MIN_ARG = 1;
	const std::size_t MAX_ARG = 2;

	std::size_t nbArg = getNbArg(cmd);
	std::string lastParam = getLastParam(cmd, MIN_ARG, MAX_ARG);

	if (isPartAllCmd(nbArg, lastParam))
	{
		ChanServ::getInstance().partAllChannels(senderSocket);
		return ;
	}

	std::vector<std::string> chanNameList;
	std::vector<std::string> passwordList;

	if (nbArg == 1)
	{
		chanNameList = splitTargets(lastParam);
	}
	else
	{
		chanNameList = splitTargets(cmd.arg.at(0));
		passwordList = splitTargets(lastParam);
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

bool JoinCmd::isPartAllCmd(const std::size_t nbArg, const std::string &lastParam) const
{
	return (nbArg == 1 && lastParam == "0");
}

void JoinCmd::tryConnection(const std::string &channel, const int senderSocket, const std::string *password) const
{
	if (!ChanServ::getInstance().isValidChannelName(channel))
		throw BadChanMaskException(channel);

	Channel& chan = ChanServ::getInstance().findOrCreateChannel(channel, senderSocket);

	/*
	si A
	{
		si B
			1
		else

	}
	*/
	
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
