#include "TopicCmd.hpp"
#include "UserManager.hpp"
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
	/** @todo send message to all users */
}
