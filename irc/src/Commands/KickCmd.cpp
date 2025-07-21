#include "KickCmd.hpp"

#include "command.hpp"
#include "ChanServ.hpp"
#include "UserManager.hpp"
#include "NeedMoreParamsException.hpp"
#include "ChannelPrivilegesNeeded.hpp"

void KickCmd::execute(const Command& cmd, const int senderSocket) const
{
	if (cmd.arg.size() != 2)
		throw NeedMoreParamsException(cmd.name);
	std::vector<std::string> channels = split(cmd.arg.at(0), ',');
	std::vector<std::string> users = split(cmd.arg.at(1), ',');
	if (channels.size() != 1 && channels.size() != users.size())
		throw NeedMoreParamsException(cmd.name);
	std::string message;
	if (!cmd.trailing.empty())
		message = cmd.trailing.substr(1);
	if (message.empty())
		message = "Kicked by " + UserManager::getInstance().getUserNickname(senderSocket);
	for (size_t i = 0; i < users.size(); ++i)
	{
		try
		{
			int chanNumber = i;
			if (i >= channels.size())
				chanNumber = 0;
			if (!ChanServ::getInstance().findChannel(channels.at(chanNumber)).isOperator(senderSocket))
				throw ChannelPrivilegesNeeded(channels.at(chanNumber));
			int socketKicked = UserManager::getInstance().findUserSocket(users.at(i));
			ChanServ::getInstance().kick(channels.at(chanNumber), socketKicked, senderSocket, message);
		}
		catch (const IrcException& e)
		{
			e.sendError(senderSocket);
		}
	}
}