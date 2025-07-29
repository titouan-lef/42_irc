#include "TopicCmd.hpp"

#include "command.hpp"
#include "ChanServ.hpp"
#include "NeedMoreParamsException.hpp"

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
	const std::size_t NB_ARG_MIN = 1;
	const std::size_t NB_ARG_MAX = 2;

	if (cmd.arg.size() < NB_ARG_MIN || cmd.arg.size() > NB_ARG_MAX)
		throw NeedMoreParamsException(cmd.name);
	Channel& channel = ChanServ::getInstance().findChannel(cmd.arg.at(0));
	return channel;
}

bool TopicCmd::isGetTopicCmd(const std::size_t nbArg) const
{
	const std::size_t NUM_ARG_TOPIC = 1;

	return (nbArg == NUM_ARG_TOPIC);
}

void TopicCmd::changeTopicName(const Command& cmd, const int senderSocket, Channel& channel) const
{
	channel.setTopic(cmd.arg.back(), senderSocket);
	channel.manageCmdMessage(senderSocket, cmd);
}
