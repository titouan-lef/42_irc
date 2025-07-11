#include "ListCmd.hpp"
#include "UserManager.hpp"
#include "ChanServ.hpp"
#include "NeedMoreParamsException.hpp"

#define ARG_REQUIERED 1

void ListCmd::execute(const Command& cmd, const int socket) const
{
	if (getNbArgs(cmd) == 0)
	{
		ChanServ::getInstance().listAllChannels(socket);
		return;
	}
	std::string lastParam = getLastParam(cmd, ARG_REQUIERED);
	std::vector<std::string> channels = split(lastParam, ',');
	for (size_t i = 0; i < channels.size(); ++i)
	{
		try
		{
			ChanServ::getInstance().findChannel(channels.at(i)).sendChannelSummary(socket);
		}
		catch(const IrcException& e)
		{
			e.sendError(socket);
		}
		
	}
	MsgServ::getInstance().sendNumericReply(socket, 323, "End of LIST");
}