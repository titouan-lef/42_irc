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

std::vector<std::string> PartCmd::split(const std::string& str, char delimiter) const
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