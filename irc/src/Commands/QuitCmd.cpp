#include "QuitCmd.hpp"
#include "UserManager.hpp"
#include "ChanServ.hpp"

/**
 * @brief Quit the server and send a essage to all channel if is in parameters
 * thhis funcion quit all time and throw none error.
 */

void QuitCmd::execute(const MsgServ::Command& cmd, const int socket) const
{
	std::string msg;
	if (!cmd.trailing.empty())
		msg = cmd.trailing.substr(1);
	ChanServ::getInstance().quitAllChannels(msg, socket);
	UserManager::getInstance().closeConnectionBySocket(socket);
}