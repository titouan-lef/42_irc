#include "UserCmd.hpp"

#include "command.hpp"
#include "MsgServ.hpp"
#include "Server.hpp"
#include "User.hpp"
#include "UserManager.hpp"
#include "PasswordMismatchException.hpp"
#include "NotRegisteredException.hpp"
#include "AlreadyRegistredException.hpp"
#include "NeedMoreParamsException.hpp"

UserCmd::UserCmd(): ACommand(4, 4) {}

void UserCmd::execute(const Command &cmd, const int senderSocket) const
{
	User& user = UserManager::getInstance().getUser(senderSocket);
	checkCmd(cmd, user);
	UpdateUser(user, cmd.arg.at(0), cmd.arg.at(2));
	sendHandshakeValidation(senderSocket);
}

void UserCmd::checkCmd(const Command& cmd, const User& user) const
{
	if (!user.isConnected())
		throw PasswordMismatchException();
	if (!user.isNickRegister())
		throw NotRegisteredException();
	checkArgSize(cmd);
	if (!validParam(cmd.arg))
		throw NeedMoreParamsException(cmd.name); /** @todo parse "0 *" */
	if (!validUsername(cmd.arg.at(0)))
		throw NeedMoreParamsException(cmd.name);
	if (user.isUsernameRegister())
		throw AlreadyRegistredException();
	/** @todo parse real name ? */
}

/**
 * @brief Verifie if the mode are "0" and "*" if is not return false
 */
bool UserCmd::validParam(const std::vector<std::string>& param) const
{
	return (param.at(1) == "0" && param.at(2) == "*");
}

/** @todo tcheck  */
bool UserCmd::validUsername(const std::string& username) const
{
	for (size_t i = 0; i < username.length(); ++i)
	{
		unsigned char c = username[i];
		if (!isalnum(c))
			return false;
	}
	return true;
}

void UserCmd::UpdateUser(User& user, const std::string& username, const std::string& realName) const
{
	user.setUsername(username);
	user.setRealName(realName);
}

void UserCmd::sendHandshakeValidation(const int socket) const
{
	const int WELCOME_CODE = 1;
	const int HOST_CODE = 2;
	const int DATE_CODE = 3;
	const int MODE_CODE = 4;

	std::string prefix = UserManager::getInstance().getUserPrefix(socket);
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

