#include "KickCmd.hpp"

#include "ChanServ.hpp"
#include "UserManager.hpp"
#include "NeedMoreParamsException.hpp"
#include "ChannelPrivilegesNeeded.hpp"
#include "NoSuchNickException.hpp"

void KickCmd::execute(const Command& cmd, const int senderSocket) const
{
	if (cmd.arg.size() < 2 || cmd.arg.size() > 3 || !cmd.trailing.empty())
		throw NeedMoreParamsException(cmd.name);
	std::vector<std::string> channels = split(cmd.arg.at(0), ',');
	std::vector<std::string> users = split(cmd.arg.at(1), ',');
	if (channels.size() != 1 && channels.size() != users.size())
		throw NeedMoreParamsException(cmd.name);
	Command replyCmd;
	replyCmd.name = "KICK";
	if (cmd.arg.size() == 2)
		replyCmd.trailing = cmd.arg.at(1);
	for (size_t i = 0; i < users.size(); ++i)
	{
		int socketKicked = UserManager::getInstance().findUserSocket(users.at(i));
		try
		{
			int chanNumber = i;
			if (i > channels.size())
				chanNumber = 0;
			if (!ChanServ::getInstance().findChannel(channels.at(chanNumber)).isOperator(senderSocket))
				throw ChannelPrivilegesNeeded(channels.at(chanNumber));
			if (socketKicked < 0)
				throw NoSuchNickException(users.at(i));
			ChanServ::getInstance().logOut(channels.at(chanNumber), replyCmd, socketKicked); /** @todo FIX: will send error message to the tagert if fails and not the sender */
		}
		catch (const IrcException& e)
		{
			e.sendError(senderSocket);
		}
	}
}