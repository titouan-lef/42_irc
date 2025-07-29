#include "QuitCmd.hpp"

#include "command.hpp"
#include "UserManager.hpp"
#include "ChanServ.hpp"

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