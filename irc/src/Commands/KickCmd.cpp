#include "KickCmd.hpp"

#include "command.hpp"
#include "status.hpp"
#include "ChanServ.hpp"
#include "UserManager.hpp"
#include "NeedMoreParamsException.hpp"
#include "ChannelPrivilegesNeeded.hpp"

KickCmd::KickCmd(): ACommand(CONNECTED, CONNECTED, 2, 3) {}

/**
 * @brief Executes the KICK command to invite a user to a channel.
 *
 * @param cmd The command object containing the kick request and its arguments.
 * @param senderSocket The socket descriptor of the user sending the kick.
 * @param userStatus The sender user status.
 * 
 * @throw Connnection: PasswordMismatchException, NotRegisteredException, AlreadyRegistredException
 * @throw Command: NeedMoreParamsException, ChannelPrivilegesNeeded
 * @todo NoSuchChannelException 403, BadChanKeyException 476, UserNotInChannel 441, ERR_NOTONCHANNEL 442
 */
void KickCmd::execute(const Command& cmd, const int senderSocket, const Status& userStatus) const
{
	checkUserStatus(userStatus);
	checkArgSize(cmd);

	std::vector<std::string> channelList = splitTargets(cmd.arg.at(0));
	std::vector<std::string> userList = splitTargets(cmd.arg.at(1));
	checkCmdArg(cmd, channelList.size(), userList.size());

	const std::string message = buildMessage(cmd, senderSocket);

	for (std::size_t i = 0; i < userList.size(); ++i)
	{
		const std::string& channelName = getChannelName(i, channelList);
		kickOneChannel(senderSocket, channelName, userList.at(i), message);
	}
}

void KickCmd::checkCmdArg(const Command& cmd, const std::size_t nbChannel, const std::size_t nbUser) const
{
	const std::size_t NB_CHAN_ALWAYS_OK = 1;

	if (nbChannel == NB_CHAN_ALWAYS_OK)
		return;
	if (nbChannel != nbUser)
		throw NeedMoreParamsException(cmd.name);
}

const std::string KickCmd::buildMessage(const Command& cmd, const int senderSocket) const
{
	if (cmd.arg.size() == nbArgMax_)
		return cmd.arg.at(2);
	const std::string& nickname = UserManager::getInstance().findUserNickname(senderSocket);
	return ("Kicked by " + nickname);
}

const std::string& KickCmd::getChannelName(const std::size_t userIndex, const std::vector<std::string>& channelList) const
{
	const std::size_t channelIndex = (userIndex >= channelList.size() ? 0 : userIndex);
	return channelList.at(channelIndex);
}

void KickCmd::kickOneChannel(const int senderSocket, const std::string& channelName, const std::string& nickname, const std::string& message) const
{
	try
	{
		checkChannel(senderSocket, channelName);
		int socketKicked = UserManager::getInstance().findUserSocket(nickname);
		ChanServ::getInstance().kick(channelName, socketKicked, senderSocket, message);
	}
	catch (const IrcException& e)
	{
		e.sendError(senderSocket);
	}
}

void KickCmd::checkChannel(const int senderSocket, const std::string& channelName) const
{
	const Channel& channel = ChanServ::getInstance().findChannel(channelName);
	if (!channel.isOperator(senderSocket))
		throw ChannelPrivilegesNeeded(channelName);
}
