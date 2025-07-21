#include "TopicCmd.hpp"

#include "command.hpp"
#include "ChanServ.hpp"
#include "NeedMoreParamsException.hpp"

void TopicCmd::execute(const Command& cmd, const int senderSocket) const
{
	Channel& channel = checkCmd(cmd);
	if (isGetTopicCmd(cmd.trailing))
		channel.sendActualTopic(senderSocket);
	else
		changeTopicName(cmd, senderSocket, channel);
}

Channel& TopicCmd::checkCmd(const Command& cmd) const
{
	const std::size_t NB_ARG = 1;

	if (cmd.arg.size() != NB_ARG)
		throw NeedMoreParamsException(cmd.name);
	Channel& channel = ChanServ::getInstance().findChannel(cmd.arg.at(0));
	return channel;
}

bool TopicCmd::isGetTopicCmd(const std::string &trailing) const
{
	return trailing.empty();
}

void TopicCmd::changeTopicName(const Command& cmd, const int senderSocket, Channel& channel) const
{
	const std::string trailing = truncateTrailing(cmd.trailing);
	channel.setTopic(trailing, senderSocket);
	channel.manageCmdMessage(senderSocket, cmd.name, cmd.arg, trailing);
}
