#include "JoinCmd.hpp"

#include <sstream>

#include "ChanServ.hpp"
#include "NeedMoreParamsException.hpp"
#include "BadChanMaskException.hpp"

void JoinCmd::execute(const Command& cmd, const int socket) const
{
	if (cmd.arg.empty() || cmd.arg.size() > 2 || !cmd.trailing.empty())
		throw NeedMoreParamsException(cmd.name);
	if (cmd.arg.size() == 1 && cmd.arg.at(0) == "0")
	{
		ChanServ::getInstance().quitAllChannels(socket);
		return ;
	}
	std::vector<std::string> channels = splitArgument(cmd.arg.at(0));
	std::vector<std::string> passwords;
	if (cmd.arg.size() == 2)
		passwords = splitArgument(cmd.arg.at(1));

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
			tryConnection(channels.at(i), socket, pwd);
		}
		catch (const IrcException& e)
		{
			e.sendError(socket);
		}
	}
}

void JoinCmd::tryConnection(const std::string& channel, const int socket, const std::string* password) const
{
	if (!isValidChannelName(channel))
		throw BadChanMaskException(channel);

	Channel& chan = ChanServ::getInstance().findOrCreateChannel(channel, socket);
	
	if (chan.isPasswordProtected())
	{
		if (password)
			chan.connectUser(socket, *password);
		else
			chan.connectUser(socket, "");
	}
	else if (!chan.isConnected(socket))
	{
		chan.connectUser(socket);
	}
}

/**
 * @brief Function split argument on ,
 * 
 * @return a vector of string split on ','.
 */
std::vector<std::string> JoinCmd::splitArgument(const std::string& arg) const
{
	std::vector<std::string>elem;
	std::string token;
	std::istringstream iss(arg);

	while (std::getline(iss, token, ','))
	{
		elem.push_back(token);
	}
	return (elem);
}

bool JoinCmd::isValidChannelName(const std::string& channelName) const
{
	if (channelName.empty() || channelName.size() > 50)
		return false;
	if (channelName.find_first_of("#&+!") != 0)
		return false;
	if (channelName.find_first_of(",\a") != std::string::npos)
		return false;
	return true;
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
