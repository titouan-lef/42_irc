#include "NamesCmd.hpp"
#include "UserManager.hpp"
#include "ChanServ.hpp"
#include "NeedMoreParamsException.hpp"

#define ARG_REQUIERED 1

void NamesCmd::execute(const Command& cmd, const int socket) const
{
	if (getNbArgs(cmd) == 0)
	{
		ChanServ::getInstance().namesAllChannels(socket);
		return;
	}
	std::string lastParam = getLastParam(cmd, ARG_REQUIERED);
	std::vector<std::string> channels = split(lastParam, ',');
	for (size_t i = 0; i < channels.size(); ++i)
	{
		ChanServ::getInstance().findChannel(channels.at(i)).sendActualUsers(socket);
	}
}
