#include "PartCmd.hpp"

#include "ChanServ.hpp"
#include "UserManager.hpp"
#include "NeedMoreParamsException.hpp"

void PartCmd::execute(const Command& cmd, const int socket) const
{
	if (cmd.arg.size() != 1)
		throw NeedMoreParamsException(cmd.name);
	Command replyCmd;
	replyCmd.name = "PART";
	if (!cmd.trailing.empty())
		replyCmd.trailing = cmd.trailing;
	std::vector<std::string> channels = split(cmd.arg.at(0), ',');
	for (size_t i = 0; i < channels.size(); ++i)
	{
		try
		{
			ChanServ::getInstance().logOut(channels.at(i), replyCmd, socket);
		}
		catch (const IrcException& e)
		{
			e.sendError(socket);
		}
	}
}
