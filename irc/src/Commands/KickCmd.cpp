#include "KickCmd.hpp"
#include "ChanServ.hpp"
#include "UserManager.hpp"
#include "NeedMoreParamsException.hpp"
#include "ChannelPrivilegesNeeded.hpp"
#include "NoSuchNickException.hpp"

void KickCmd::execute(const MsgServ::Command& cmd, const int socket) const
{
	if (cmd.arg.size() < 2 || cmd.arg.size() > 3)
		throw NeedMoreParamsException(socket, cmd.name, UserManager::getInstance().getUserNickname(socket));
	std::vector<std::string> channels = split(cmd.arg.at(0), ',');
	std::vector<std::string> users = split(cmd.arg.at(1), ',');
	if (channels.size() != 1 || channels.size() != users.size())
		throw NeedMoreParamsException(socket, cmd.name, UserManager::getInstance().getUserNickname(socket));
	std::string message;
	if (cmd.arg.size() == 2)
		message = cmd.arg.at(1);
	for (size_t i = 0; i < users.size(); ++i)
	{
		int socketKicked = UserManager::getInstance().findUserSocket(users.at(i));
		try
		{
			if (!ChanServ::getInstance().findChannel(channels.at(0), socket).isModerator(socket))
				throw ChannelPrivilegesNeeded(socket, channels.at(0));
			if (socketKicked < 0)
				throw NoSuchNickException(socket, users.at(i));
			if (i > channels.size())
				ChanServ::getInstance().logOut(channels.at(0), message, socketKicked); /** @todo FIX: will send error message to the tagert if fails */
			else
				ChanServ::getInstance().logOut(channels.at(i), message, socketKicked);
		}
		catch (const IrcException& e)
		{
			e.sendError();
		}
	}
}

std::vector<std::string> KickCmd::split(const std::string& str, char delimiter) const
{
	std::vector<std::string> tokens;
	std::string::size_type start = 0;
	std::string::size_type end;

	while ((end = str.find(delimiter, start)) != std::string::npos)
	{
		tokens.push_back(str.substr(start, end - start));
		start = end + 1;
	}
	tokens.push_back(str.substr(start));
	return tokens;
}