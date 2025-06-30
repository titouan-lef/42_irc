#include "CommandManager.hpp"

#include <sstream>

#include "MsgServ.hpp"
#include "InviteCmd.hpp"
#include "JoinCmd.hpp"
#include "KickCmd.hpp"
#include "ModeCmd.hpp"
#include "NickCmd.hpp"
#include "PartCmd.hpp"
#include "PassCmd.hpp"
#include "PingCmd.hpp"
#include "PongCmd.hpp"
#include "PrivmsgCmd.hpp"
#include "QuitCmd.hpp"
#include "TopicCmd.hpp"
#include "UserCmd.hpp"
#include "UnknowCmdException.hpp"
#include "NotRegisteredException.hpp"
#include "PasswordMismatchException.hpp"

/**
 * @details * This constructor initializes the `commands_` map by inserting the various IRC commands
* supported by the server. Each command (such as INVITE, JOIN, KICK, etc.)
* is associated with an instance of an object derived from the base command class.
 */
CommandManager::CommandManager()
{
	commands_["INVITE"] = new InviteCmd();
	commands_["JOIN"] = new JoinCmd();
	commands_["KICK"] = new KickCmd();
	commands_["MODE"] = new ModeCmd();
	commands_["NICK"] = new NickCmd();
	commands_["PART"] = new PartCmd();
	commands_["PASS"] = new PassCmd();
	commands_["PING"] = new PingCmd();
	commands_["PONG"] = new PongCmd();
	commands_["PRIVMSG"] = new PrivmsgCmd();
	commands_["QUIT"] = new QuitCmd();
	commands_["TOPIC"] = new TopicCmd();
	commands_["USER"] = new UserCmd();
}

CommandManager::~CommandManager()
{
	for (std::map<std::string, ACommand*>::iterator it = commands_.begin(); it != commands_.end(); ++it)
		delete it->second;
}

CommandManager& CommandManager::getInstance()
{
	static CommandManager instance;
	return instance;
}

/*
* @brief Interprets and processes data received from a user.
*
* This method parses the raw command received (as a string),
* then performs the appropriate action based on the user's state.
*/
void CommandManager::interpretData(const std::string& recv, const int socket)
{
	MsgServ::Command cmd = parserCmd(recv, socket);
	commands_[cmd.name]->execute(cmd, socket);
}

/**
* @brief Parses a raw command string and extracts its components.
*
* This function takes a command line received from a client (as a string),
* and breaks it into several parts:
* - `cmd.name`: the main command ("JOIN", "NICK", etc.)
* - `cmd.arg`: a space-separated list of arguments
* - `cmd.trailing`: the "free" data at the end of the command, preceded by a `:`,
* often used for messages with spaces (e.g., a message in PRIVMSG)
*
* If the command is not recognized (i.e., not present in `commands_`),
* an `UnknowCmdException` exception is thrown.
*
* @param recv The raw command sent by the client (e.g., "PRIVMSG #channel :Hello all").
*
* @return An `MsgServ::Command` structure containing the parsed command.
*
* @throws UnknowCmdException if the command is not registered in `commands_`.
*/
MsgServ::Command CommandManager::parserCmd(const std::string& recv, const int socket)
{
	MsgServ::Command cmd;
	std::string token;
	std::istringstream iss;
	iss.str(recv);

	while (std::getline(iss, cmd.name, ' ') && cmd.name.empty())
		continue ;
	if (commands_.find(cmd.name) == commands_.end())
		throw UnknowCmdException(socket, cmd.name);
	while (std::getline(iss, token, ' '))
	{
		if (token.empty())
			continue ;
		if (token[0] == ':')
		{
			cmd.trailing = token;
			std::string rest;
			std::getline(iss, rest);
			cmd.trailing += rest;
			break;
		}
		cmd.arg.push_back(token);
	}
	return (cmd);
}

/**
 * @brief Handles the progressive registration of an IRC client.
 *
 * This function is called when the user is not yet fully registered.
 * It determines, based on the user's state (connection, nickname, etc.),
 * whether the received command is valid in the context of registration, and executes it.
 *
 * The registration typically follows three steps: PASS → NICK → USER.
 *
 * @param cmd The command to process, in structured form.
 * @param user Reference to the user concerned by the registration process.
 *
 * @throws PasswordMismatchException If a command other than PASS is sent before authentication.
 * @throws NotRegisteredException If a disallowed command is sent during the registration phase.
 */
void CommandManager::registerClient(const std::string& recv, const int socket)
{
	MsgServ::Command cmd = parserCmd(recv, socket);
	if (cmd.name != "USER" && cmd.name != "NICK" && cmd.name != "PASS")
		throw NotRegisteredException(socket);
	commands_[cmd.name]->execute(cmd, socket);
}
