#include "TopicCmd.hpp"

#include "command.hpp"
#include "ChanServ.hpp"
#include "NeedMoreParamsException.hpp"

void TopicCmd::execute(const Command& cmd, const int senderSocket) const
{
	if (cmd.arg.size() != 1)
		throw NeedMoreParamsException(cmd.name);
	Channel& channel = ChanServ::getInstance().findChannel(cmd.arg.at(0));
	if (cmd.trailing.empty())
	{
		channel.sendActualTopic(senderSocket);
		return;
	}
	channel.setTopic(cmd.trailing.substr(1), senderSocket);
	std::string trailing;
	if (!cmd.trailing.empty())
		trailing = cmd.trailing.substr(1);
	channel.manageCmdMessage(senderSocket, cmd.name, cmd.arg, trailing);
}
