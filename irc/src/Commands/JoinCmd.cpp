#include "JoinCmd.hpp"

#include "ChanServ.hpp"
#include "Channel.hpp"
#include "BadChanMaskException.hpp"
#include "NeedMoreParamsException.hpp"
#include "command.hpp"
#include "status.hpp"

JoinCmd::JoinCmd(): ACommand(CONNECTED, CONNECTED, 1, 2) {}


/**
 * @brief Executes the JOIN command to join or create a channel if is doesn't exist.
 *
 * @param cmd The command object containing the join request and its arguments.
 * @param senderSocket The socket descriptor of the user sending join.
 * @param userStatus The sender user status.
 * 
 * @throw Connnection: PasswordMismatchException, NotRegisteredException, AlreadyRegistredException
 * @throw Command: NeedMoreParamsException, UserOnChannelException, NoSuchNickException, BadChanMaskException
 * @todo  execption forbiden : ERR_INVITEONLYCHAN 473, ERR_BADCHANNELKEY 475, ERR_CHANNELISFULL 471, ERR_NOSUCHCHANNEL 403/ maybe KeySet(467) ?
 */
void JoinCmd::execute(const Command& cmd, const int senderSocket, const Status& userStatus) const
{
	checkUserStatus(userStatus);
	checkArgSize(cmd);
	if (isPartAllChannelCmd(cmd.arg))
		partAllChannel(cmd, senderSocket);
	else
		joinChannel(cmd, senderSocket);
}

bool JoinCmd::isPartAllChannelCmd(const std::vector<std::string> &arg) const
{
	return (arg.at(0) == "0");
}

void JoinCmd::partAllChannel(const Command &cmd, const int senderSocket) const
{
	if (cmd.arg.size() != nbArgMin_)
		throw NeedMoreParamsException(cmd.name);
	ChanServ::getInstance().partAllChannels(senderSocket);
}

void JoinCmd::joinChannel(const Command &cmd, const int senderSocket) const
{
	const std::vector<std::string> chanNameList = splitTargets(cmd.arg.at(0));
	const std::vector<std::string> passwordList = definePassword(cmd);
	connectToAllChannel(senderSocket, chanNameList, passwordList);
}

std::vector<std::string> JoinCmd::definePassword(const Command &cmd) const
{
	std::vector<std::string> passwordList;
	if (cmd.arg.size() == nbArgMax_)
		passwordList = splitTargets(cmd.arg.at(1));
	return passwordList;
}

void JoinCmd::connectToAllChannel(const int senderSocket, const std::vector<std::string> &chanNameList, const std::vector<std::string> &passwordList) const
{
	std::size_t indexPassword = 0;
	for (std::size_t i = 0; i < chanNameList.size(); ++i)
		connectToOneChannel(senderSocket, chanNameList.at(i), passwordList, indexPassword);
}

void JoinCmd::connectToOneChannel(const int senderSocket, const std::string& chanName, const std::vector<std::string>& passwordList, std::size_t& currentIndex) const
{
	Channel* chan = ChanServ::getInstance().searchChannel(chanName);
	try
	{
		if (chan == NULL)
			connectToNewChannel(senderSocket, chanName);
		else
			connection(*chan, senderSocket, passwordList, currentIndex);
	}
	catch(const IrcException& e)
	{
		e.sendError(senderSocket);
	}
	
}

void JoinCmd::connectToNewChannel(const int senderSocket, const std::string &chanName) const
{
	if (!ChanServ::getInstance().isValidChannelName(chanName))
		throw BadChanMaskException(chanName);
	ChanServ::getInstance().createChannelWithOperator(chanName, senderSocket);
}

void JoinCmd::connection(Channel& chan, const int senderSocket, const std::vector<std::string>& passwordList, std::size_t& currentIndex) const
{
	const std::string& password = getNextPassword(chan, passwordList, currentIndex);
	chan.connectUser(senderSocket, password);
}

const std::string JoinCmd::getNextPassword(const Channel& chan, const std::vector<std::string>& passwordList, std::size_t& currentIndex) const
{
	if (currentIndex >= passwordList.size() || !chan.isPasswordProtected())
		return "";
	++currentIndex;
	return passwordList.at(currentIndex - 1);
}
