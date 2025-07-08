#include "PrivmsgCmd.hpp"
#include "MsgServ.hpp"
#include "ChanServ.hpp"
#include "UserManager.hpp"
#include "NoSuchNickException.hpp"
#include "NeedMoreParamsException.hpp"
#include "NoTextToSendException.hpp"
#include "TooManyTargetsException.hpp"
#define MAX_TARGET 4

void PrivmsgCmd::execute(const Command& cmd, const int senderSocket) const
{
	if (cmd.arg.size() != 1)
		throw NeedMoreParamsException(cmd.name);
	if (cmd.trailing.empty() || cmd.trailing == ":")
		throw NoTextToSendException();
	std::vector<std::string>target = split(cmd.arg.at(0), ',');
	if (target.size() > 4)
		throw TooManyTargetsException(cmd.arg.at(0));
	Command truncateCmd = truncateTrailing(cmd);
	for (size_t i = 0; target.size() > i; i++)
	{
		if (!ChanServ::getInstance().isValidChannelName(target.at(i)))
		{
			Channel chanTmp = ChanServ::getInstance().findChannel(target.at(i));
			chanTmp.sendMessageToAllUsers(senderSocket, truncateCmd);
		}
		else
		{
			int receiverSocket = UserManager::getInstance().findUserSocket(target.at(i));
			if (receiverSocket == -1)
			throw NoSuchNickException(truncateCmd.arg.at(i));
			MsgServ::getInstance().sendMessageReply(senderSocket, receiverSocket, truncateCmd.name, truncateCmd.arg, truncateCmd.trailing);
		}
	}
}