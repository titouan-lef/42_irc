#include "ListCmd.hpp"
#include "ChanServ.hpp"
#include "NeedMoreParamsException.hpp"
#include "UserManager.hpp"

void ListCmd::execute(const MsgServ::Command& cmd, const int socket) const
{

	if (cmd.arg.empty())
	{
		ChanServ::getInstance().listAllChannels(socket);
		return;
	}
	if (cmd.arg.size() > 1)
		throw NeedMoreParamsException(socket, cmd.name, UserManager::getInstance().getUserNickname(socket));
	std::vector<std::string> channels = split(cmd.arg.at(0), ',');
	for (size_t i = 0; i < channels.size(); ++i)
	{
		ChanServ::getInstance().findChannel(channels.at(i), socket).sendChannelSummary(socket);
	}
}

std::vector<std::string> ListCmd::split(const std::string& str, char delimiter) const
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