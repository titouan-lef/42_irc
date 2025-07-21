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
	checkCmdParam(cmd);
	const std::string& nicknameAllTarget = cmd.arg.at(0);
	std::vector<std::string> targetList = splitTargets(nicknameAllTarget);
	checkNbTarget(targetList.size(), nicknameAllTarget);
	for (size_t i = 0; targetList.size() > i; i++)
		sendToTarget(cmd, senderSocket, targetList.at(i));
}

void PrivmsgCmd::checkCmdParam(const Command& cmd) const
{
	const std::size_t NB_ARG = 1;

	if (cmd.arg.size() > NB_ARG)
		throw NeedMoreParamsException(cmd.name);
	if (cmd.arg.size() < NB_ARG)
		throw NoRecipientException(cmd.name);
	if (cmd.trailing.empty() || cmd.trailing == ":")
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
		sendMsgToService(cmd.trailing, senderSocket, target);
	else
		sendMsgToUser(cmd, senderSocket, target);
}

bool PrivmsgCmd::isMsgToChannel(const std::string &target) const { return ChanServ::getInstance().isValidChannelName(target); }

void PrivmsgCmd::sendMsgToChannel(const Command &cmd, const int senderSocket, const std::string& channelName) const
{
	const Channel& channel = ChanServ::getInstance().findChannel(channelName);
	const std::string truncTrailing =  truncateTrailing(cmd.trailing);
	channel.sendMessageToAllUsers(senderSocket, cmd.name, cmd.arg, truncTrailing);
}

bool PrivmsgCmd::isMsgToService(const std::string &target) const { return UserManager::getInstance().isServiceName(target); }

void PrivmsgCmd::sendMsgToService(const std::string &trailing, const int senderSocket, const std::string &serviceName) const
{
	const std::string truncTrailing =  truncateTrailing(trailing);
	UserManager::getInstance().sendCommandToService(senderSocket, serviceName, truncTrailing);
}

void PrivmsgCmd::sendMsgToUser(const Command &cmd, const int senderSocket, const std::string &nickname) const
{
	int receiverSocket = UserManager::getInstance().findUserSocket(nickname);
	const std::string truncTrailing =  truncateTrailing(cmd.trailing);
	MsgServ::getInstance().sendMessageReply(senderSocket, receiverSocket, cmd.name, cmd.arg, truncTrailing);
}
