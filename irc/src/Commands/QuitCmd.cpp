#include "QuitCmd.hpp"

#include "UserManager.hpp"
#include "ChanServ.hpp"
#include "command.hpp"
#include "status.hpp"

QuitCmd::QuitCmd(): ACommand(CONNECTED, CONNECTED, 0, -1) {}

/**
 * @brief Executes the QUIT command to exit Quit the server and send a essage to all channel if is in parameters
 * 
 * @param cmd The command object containing the quit request and its arguments.
 * @param socket The socket descriptor of the user attempting to authenticate.
 * @param userStatus The sender user status.
 *
 * @throws Connnection: PasswordMismatchException, NotRegisteredException, AlreadyRegistredException
 * @todo need throw if is not connect.
 */
void QuitCmd::execute(const Command& cmd, const int senderSocket, const Status& userStatus) const
{
	checkUserStatus(userStatus);
	const char* msg = (cmd.arg.empty() ? NULL : cmd.arg.at(0).c_str());/** @todo param 0 is the message ? */
	ChanServ::getInstance().quit(senderSocket, msg);
	UserManager::getInstance().closeConnectionBySocket(senderSocket);
}