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
	if (cmd.arg.size() < 1 || cmd.arg.size() > 2 || !cmd.trailing.empty())
		throw NeedMoreParamsException(socket, cmd.name, user.getNickname());
	if (cmd.arg.size() == 1 && cmd.arg[0] == "0")
	{
		ChanServ::getInstance().quitAllChannels("", socket);
		return ;
	}
	std::vector<std::string> channels = splitArgument(cmd.arg[0]);
	if (cmd.arg.size() == 1)
	{
		for (size_t i = 0; i < channels.size(); i++)
		{
			try
			{
				tryConnection(channels[i], socket);
			}
			catch(const IrcException& e)
			{
				e.sendError();
			}
		}
	}
	else
	{
		std::vector<std::string>passwords = splitArgument(cmd.arg[1]);
		std::vector<std::string>::iterator itPwd = passwords.begin();
		for (size_t i = 0; i < channels.size(); i++)
		{
			try
			{ 
				if (itPwd > passwords.end())
				{
					std::string invalidPassword;
					tryConnection(channels[i], invalidPassword, socket);
				}
				else
					tryConnection(channels[i], itPwd, socket);
			}
			catch(const IrcException& e)
			{
				e.sendError();
			}
		}
	}

}

void JoinCmd::tryConnection(const std::string& channel, const int socket) const
{
	if (checkChannelName(channel))
		throw BadChanMaskException(socket, channel);
	Channel tmpChan = ChanServ::getInstance().findOrCreateChannel(channel, socket);
	if (!tmpChan.isConnected(socket))
		tmpChan.connectUser(socket);
}

void JoinCmd::tryConnection(const std::string& channel, std::vector<std::string>::iterator& itPwd, const int socket) const
{
	if (checkChannelName(channel))
		throw BadChanMaskException(socket, channel);
	Channel tmpChan = ChanServ::getInstance().findOrCreateChannel(channel, socket);
	if (tmpChan.isPasswordProtected())
	{
		tmpChan.connectUser(socket, *itPwd);
		itPwd++;
	}
	else if (!tmpChan.isConnected(socket))
		tmpChan.connectUser(socket);
}

void JoinCmd::tryConnection(const std::string& channel, std::string invalidPassword, const int socket) const
{
	if (checkChannelName(channel))
		throw BadChanMaskException(socket, channel);
	Channel tmpChan = ChanServ::getInstance().findOrCreateChannel(channel, socket);
	if (tmpChan.isPasswordProtected())
	{
		tmpChan.connectUser(socket, invalidPassword);
	}
	else if (!tmpChan.isConnected(socket))
		tmpChan.connectUser(socket);
}

std::vector<std::string> JoinCmd::splitArgument(const std::string& arg) const
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

bool JoinCmd::checkChannelName(const std::string& channelName) const
{
	if (channelName.size() < 1 || channelName.size() > 50)
		return true;
	if (std::string("#&+!").find(channelName[0]) == std::string::npos)
		return true;
	if (!(channelName.find(',') == std::string::npos) || !(channelName.find(32) == std::string::npos) || !(channelName.find(7) == std::string::npos))
		return true;
	return false;
}