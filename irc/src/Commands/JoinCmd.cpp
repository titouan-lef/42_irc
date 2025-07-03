#include <iostream>
#include <string>
#include <sstream>
#include "JoinCmd.hpp"
#include "UserManager.hpp"
#include "ChanServ.hpp"
#include "NeedMoreParamsException.hpp"
#include "BadChanMaskException.hpp"

void JoinCmd::execute(const MsgServ::Command& cmd, const int socket) const
{
	User &user = UserManager::getInstance().getUser(socket);
	if (cmd.arg.size() < 1 && cmd.arg.size() > 2)
		throw NeedMoreParamsException(socket, cmd.name, user.getNickname());
	std::vector<std::string>channels = splitArgument(cmd.arg[0]);
	std::vector<std::string>passwords = splitArgument(cmd.arg[1]);
	std::vector<std::string>::iterator itPassword = passwords.begin();
	for (size_t i = 0; i < channels.size(); i++)
	{
		if (checkChannelName(channels[i]))
			throw BadChanMaskException(socket, channels[i]);
		Channel tmpChan = ChanServ::getInstance().findChannelFor(channels[i], socket);
		if (tmpChan.havePassword())
		{
			tmpChan.connectUser(socket, *itPassword);
			itPassword++;
		}
		else
			tmpChan.connectUser(socket);
	}
}

std::vector<std::string> splitArgument(const std::string& arg)
{
	std::vector<std::string>elem;
	std::string token;
	std::istringstream iss;
	iss.str(arg);

	while (std::getline(iss, token, ','))
	{
		elem.push_back(token);
	}
	return (elem);
}

bool checkChannelName(const std::string& channelName)
{
	if (channelName.size() < 1 || channelName.size() > 50)
		return true;
	if (std::string("#&+!").find(channelName[0]) == std::string::npos)
		return true;
	if (channelName.find(',') || channelName.find(32) || channelName.find(7))
		return true;
	return false;
}