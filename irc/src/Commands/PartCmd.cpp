#include "PartCmd.hpp"

#include "ChanServ.hpp"
#include "UserManager.hpp"
#include "NeedMoreParamsException.hpp"

void PartCmd::execute(const Command& cmd, const int socket) const
{
	if (cmd.arg.size() < 1 || cmd.arg.size() > 2 || !cmd.trailing.empty())
		throw NeedMoreParamsException(cmd.name);
	std::vector<std::string> channels = split(cmd.arg.at(0), ',');
	Command replyCmd;
	replyCmd.name = "PART";
	if (cmd.arg.size() == 2)
		replyCmd.trailing = cmd.arg.at(1);
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
