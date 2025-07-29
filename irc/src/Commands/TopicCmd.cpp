#include "TopicCmd.hpp"

#include "command.hpp"
#include "ChanServ.hpp"

TopicCmd::TopicCmd(): ACommand(1, 2) {}

void TopicCmd::execute(const Command& cmd, const int senderSocket) const
{
	Channel& channel = checkCmd(cmd);
	if (isGetTopicCmd(cmd.arg.size()))
		channel.sendActualTopic(senderSocket);
	else
		changeTopicName(cmd, senderSocket, channel);
}

Channel& TopicCmd::checkCmd(const Command& cmd) const
{
	checkArgSize(cmd);
	Channel& channel = ChanServ::getInstance().findChannel(cmd.arg.at(0));
	return channel;
}

bool TopicCmd::isGetTopicCmd(const std::size_t nbArg) const
{
	return (nbArg == nbArgMin_);
}

void TopicCmd::changeTopicName(const Command& cmd, const int senderSocket, Channel& channel) const
{
	channel.setTopic(cmd.arg.at(nbArgMax_ - 1), senderSocket);
	channel.manageCmdMessage(senderSocket, cmd);
}
