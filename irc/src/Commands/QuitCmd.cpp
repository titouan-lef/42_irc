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
	std::string msg;
	if (!cmd.trailing.empty())
		msg = cmd.trailing.substr(1);
	ChanServ::getInstance().quit(senderSocket, msg);
	UserManager::getInstance().closeConnectionBySocket(senderSocket);
}