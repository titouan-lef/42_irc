#include "PrivmsgCmd.hpp"

#include "command.hpp"
#include "MsgServ.hpp"
#include "ChanServ.hpp"
#include "UserManager.hpp"
#include "NeedMoreParamsException.hpp"
#include "NoTextToSendException.hpp"
#include "TooManyTargetsException.hpp"
#include "NoRecipientException.hpp"

void PrivmsgCmd::execute(const Command& cmd, const int senderSocket) const
{
	if (cmd.arg.size() > 1)
		throw NeedMoreParamsException(cmd.name);
	if (cmd.arg.size() < 1)
		throw NoRecipientException(cmd.name);
	if (cmd.trailing.empty() || cmd.trailing == ":")
		throw NoTextToSendException();
	std::vector<std::string>target = split(cmd.arg.at(0), ',');
	if (target.size() > 4)
		throw TooManyTargetsException(cmd.arg.at(0));
	for (size_t i = 0; target.size() > i; i++)
	{
		if (ChanServ::getInstance().isValidChannelName(target.at(i)))
		{
			Channel chanTmp = ChanServ::getInstance().findChannel(target.at(i));
			chanTmp.sendMessageToAllUsers(senderSocket, cmd.name, cmd.arg, truncateTrailing(cmd.trailing));
		}
		else if (UserManager::getInstance().isServiceName(target.at(i)))
			UserManager::getInstance().sendCommandToService(senderSocket, target.at(i), truncateTrailing(cmd.trailing));
		else
		{
			int receiverSocket = UserManager::getInstance().findUserSocket(target.at(i));
			MsgServ::getInstance().sendMessageReply(senderSocket, receiverSocket, cmd.name, cmd.arg, truncateTrailing(cmd.trailing));
		}
	}
}