#include "QuitCmd.hpp"

#include "UserManager.hpp"
#include "ChanServ.hpp"
#include "command.hpp"

QuitCmd::QuitCmd(): ACommand(0, -1) {}

/**
 * @brief Execute Quit command. Quit the server and send a essage to all channel if is in parameters
 * thhis funcion quit all time and throw none error.
 */
void QuitCmd::execute(const Command& cmd, const int senderSocket) const
{
	const char* msg = (cmd.arg.empty() ? NULL : cmd.arg.at(0).c_str());/** @todo param 0 is the message ? */
	ChanServ::getInstance().quit(senderSocket, msg);
	UserManager::getInstance().closeConnectionBySocket(senderSocket);
}