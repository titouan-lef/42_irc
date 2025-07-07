#include "ListCmd.hpp"

#include "ChanServ.hpp"
#include "NeedMoreParamsException.hpp"

void ListCmd::execute(const Command& cmd, const int socket) const
{

	if (cmd.arg.empty())
	{
		ChanServ::getInstance().listAllChannels(socket);
		return;
	}
	if (cmd.arg.size() > 1)
		throw NeedMoreParamsException(cmd.name);
	std::vector<std::string> channels = split(cmd.arg.at(0), ',');
	for (size_t i = 0; i < channels.size(); ++i)
	{
		ChanServ::getInstance().findChannel(channels.at(i)).sendChannelSummary(socket);
	}
}
