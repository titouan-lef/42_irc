#include "TopicCmd.hpp"

#include "command.hpp"
#include "status.hpp"
#include "ChanServ.hpp"

TopicCmd::TopicCmd(): ACommand(CONNECTED, CONNECTED, 1, 2) {}

/**
 * @brief Executes the TOPIC command used to change or view the topic of a channel.
 * 
 * @param cmd The command object containing the topic request and password as the last parameter.
 * @param socket The socket descriptor of the user attempting to authenticate.
 * @param userStatus The sender user status.
 *
 * @throws Connnection: PasswordMismatchException, NotRegisteredException, AlreadyRegistredException
 * @throws Command:
 */
void TopicCmd::execute(const Command& cmd, const int senderSocket, const Status& userStatus) const
{
	checkUserStatus(userStatus);
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
