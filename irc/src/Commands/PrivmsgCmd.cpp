#include "PrivmsgCmd.hpp"

#include "command.hpp"
#include "MsgServ.hpp"
#include "ChanServ.hpp"
#include "UserManager.hpp"
#include "NeedMoreParamsException.hpp"
#include "NoTextToSendException.hpp"
#include "TooManyTargetsException.hpp"
#include "NoRecipientException.hpp"

PrivmsgCmd::PrivmsgCmd(): ACommand(2, 2) {}

void PrivmsgCmd::execute(const Command& cmd, const int senderSocket) const
{
	checkCmd(cmd);
	const std::string& nicknameAllTarget = cmd.arg.at(0);
	std::vector<std::string> targetList = splitTargets(nicknameAllTarget);
	checkNbTarget(targetList.size(), nicknameAllTarget);
	for (size_t i = 0; targetList.size() > i; i++)
		sendToTarget(cmd, senderSocket, targetList.at(i));
}

void PrivmsgCmd::checkCmd(const Command& cmd) const
{
	if (cmd.arg.size() > nbArgMax_)
		throw NeedMoreParamsException(cmd.name);
	if (cmd.arg.size() < nbArgMin_)
		throw NoRecipientException(cmd.name);
	if (cmd.arg.at(1).empty())
		throw NoTextToSendException();
}

void PrivmsgCmd::checkNbTarget(const std::size_t nbTarget, const std::string& nicknameAllTarget) const
{
	const std::size_t NB_MAX_TARGET = 4;

	if (nbTarget > NB_MAX_TARGET)
		throw TooManyTargetsException(nicknameAllTarget);
}

void PrivmsgCmd::sendToTarget(const Command& cmd, const int senderSocket, const std::string &target) const
{
	if (isMsgToChannel(target))
		sendMsgToChannel(cmd, senderSocket, target);
	else if (isMsgToService(target))
		sendMsgToService(cmd.arg.at(1), senderSocket, target);
	else
		sendMsgToUser(cmd, senderSocket, target);
}

bool PrivmsgCmd::isMsgToChannel(const std::string &target) const { return ChanServ::getInstance().isValidChannelName(target); }

void PrivmsgCmd::sendMsgToChannel(const Command &cmd, const int senderSocket, const std::string& channelName) const
{
	const Channel& channel = ChanServ::getInstance().findChannel(channelName);
	channel.sendMessageToAllUsers(senderSocket, cmd);
}

bool PrivmsgCmd::isMsgToService(const std::string &target) const { return UserManager::getInstance().isServiceName(target); }

void PrivmsgCmd::sendMsgToService(const std::string &message, const int senderSocket, const std::string &serviceName) const
{
	UserManager::getInstance().sendCommandToService(senderSocket, serviceName, message);
}

void PrivmsgCmd::sendMsgToUser(const Command &cmd, const int senderSocket, const std::string &nickname) const
{
	int receiverSocket = UserManager::getInstance().findUserSocket(nickname);
	MsgServ::getInstance().sendMessageReply(senderSocket, receiverSocket, cmd);
}
