#include "UserCmd.hpp"

#include "command.hpp"
#include "status.hpp"
#include "MsgServ.hpp"
#include "Server.hpp"
#include "User.hpp"
#include "UserManager.hpp"
#include "NeedMoreParamsException.hpp"

UserCmd::UserCmd(): ACommand(IDENTIFIED, IDENTIFIED, 4, 4) {}

/**
 * @brief Executes the USER command to specify the username, hostname and realname of a new user.
 * 
 * @param cmd The command object containing the user request and its arguments.
 * @param socket The socket descriptor of the user attempting to authenticate.
 * @param userStatus The sender user status.
 *
 * @throws Connnection: PasswordMismatchException, NotRegisteredException, AlreadyRegistredException
 * @throws Command: NeedMoreParamsException
 */
void UserCmd::execute(const Command &cmd, const int senderSocket, const Status& userStatus) const
{
	checkUserStatus(userStatus);
	checkCmd(cmd);
	User& user = UserManager::getInstance().findUser(senderSocket);
	updateUser(user, cmd.arg.at(0));
	sendHandshakeValidation(senderSocket);
}

void UserCmd::checkCmd(const Command& cmd) const
{
	checkArgSize(cmd);
	if (!validParam(cmd.arg))
		throw NeedMoreParamsException(cmd.name);
	if (!validUsername(cmd.arg.at(0)))
		throw NeedMoreParamsException(cmd.name);
}

/**
 * @brief Check if the mode are "0" and "*".
 */
bool UserCmd::validParam(const std::vector<std::string>& param) const
{
	return (param.at(1) == "0" && param.at(2) == "*");
}

/** @todo tcheck  */
bool UserCmd::validUsername(const std::string& username) const
{
	if (username.empty())
		return false;
	for (std::size_t i = 0; i < username.length(); ++i)
	{
		unsigned char c = username[i];
		if (c == '\0' || c == '\r' || c == '\n' || c == ' ' || c == ':')
			return false;
	}
	return true;
}

void UserCmd::updateUser(User& user, const std::string& username) const
{
	user.setUsername(username);
	user.passToNextConnectionStep();
}

void UserCmd::sendHandshakeValidation(const int socket) const
{
	const int WELCOME_CODE = 1;
	const int HOST_CODE = 2;
	const int DATE_CODE = 3;
	const int MODE_CODE = 4;

	std::string prefix = UserManager::getInstance().findUserPrefix(socket);
	MsgServ::getInstance().sendNumericReply(socket, WELCOME_CODE, "Welcome to the Internet Relay Network " + prefix);

	MsgServ::getInstance().sendNumericReply(socket, HOST_CODE, "Your host is ircserv, running version 1.0");

	std::string date = Server::getInstance().getCreationDate();
	MsgServ::getInstance().sendNumericReply(socket, DATE_CODE, "This server was created the " + date);

	std::vector<std::string> arg;
	arg.push_back("ircserv");
	arg.push_back("1.0");
	arg.push_back("-");
	arg.push_back("itkol");
	MsgServ::getInstance().sendNumericReply(socket, MODE_CODE, arg);
}

