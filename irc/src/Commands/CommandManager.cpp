#include "CommandManager.hpp"

#include "UserManager.hpp"
#include "InviteCmd.hpp"
#include "JoinCmd.hpp"
#include "KickCmd.hpp"
#include "ListCmd.hpp"
#include "ModeCmd.hpp"
#include "NamesCmd.hpp"
#include "NickCmd.hpp"
#include "PartCmd.hpp"
#include "PassCmd.hpp"
#include "PingCmd.hpp"
#include "PongCmd.hpp"
#include "PrivmsgCmd.hpp"
#include "QuitCmd.hpp"
#include "TopicCmd.hpp"
#include "UserCmd.hpp"
#include "UnknownCmdException.hpp"
#include "NotRegisteredException.hpp"
#include "PasswordMismatchException.hpp"

/**
 * @details * This constructor initializes the 'commands_' map by inserting the various IRC commands
* supported by the server. Each command (such as INVITE, JOIN, KICK, etc.)
* is associated with an instance of an object derived from the base command class.
 */
CommandManager::CommandManager()
{
	commands_["INVITE"] = new InviteCmd();
	commands_["JOIN"] = new JoinCmd();
	commands_["KICK"] = new KickCmd();
	commands_["LIST"] = new ListCmd();
	commands_["MODE"] = new ModeCmd();
	commands_["NAMES"] = new NamesCmd();
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

/**
 * @brief Returns the singleton instance of CommandManager.
 *
 * This method ensures that only one instance of CommandManager exists
 * throughout the program lifetime.
 *
 * @return Reference to the single CommandManager instance.
 */
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
void CommandManager::interpretData(const std::string& recv, const int senderSocket)
{
	UserManager::getInstance().updateLastActivity(senderSocket);
	Command cmd = parserCmd(recv);
	commands_[cmd.name]->execute(cmd, senderSocket);
}

/**
* @brief Parses a raw command string and extracts its components.
*
* This function takes a command line received from a client (as a string),
* and breaks it into several parts:
* - 'cmd.name': the main command ("JOIN", "NICK", etc.)
* - 'cmd.arg': a space-separated list of arguments
* - 'cmd.trailing': the "free" data at the end of the command, preceded by a ':',
* often used for messages with spaces (e.g., a message in PRIVMSG)
*
* If the command is not recognized (i.e., not present in 'commands_'),
* an 'UnknownCmdException' exception is thrown.
*
* @param recv The raw command sent by the client (e.g., "PRIVMSG #channel :Hello all").
*
* @return An 'Command' structure containing the parsed command.
*
* @throws UnknownCmdException if the command is not registered in 'commands_'.
*/
Command CommandManager::parserCmd(const std::string& recv) const
{
	Command cmd;
	std::size_t startNextParam = createCmdName(recv, cmd.name);
	parserCmdElement(recv, startNextParam, cmd);
	return (cmd);
}


std::size_t CommandManager::createCmdName(const std::string& recv, std::string& cmdName) const
{
	std::size_t startNextParam = delimitParameter(recv, 0, cmdName);
	if (cmdNotExist(cmdName))
		throw UnknownCmdException(cmdName);
	return startNextParam;
}

/**
 * @brief Checks whether a command does not exist or has an empty name.
 * This function returns 'true' if the command name is an empty string
 * or if no command with the given name is found in the 'commands_' container.
 *
 * @param cmdName The name of the command to check.
 *
 * @return 'true' if the command is invalid or not found, 'false' otherwise.
 */
bool CommandManager::cmdNotExist(const std::string &cmdName) const
{
	return (cmdName.empty() || commands_.find(cmdName) == commands_.end());
}

void CommandManager::parserCmdElement(const std::string& recv, std::size_t start, Command& cmd) const
{
	while (start != recv.size())
	{
		std::string param;
		start = delimitParameter(recv, start, param);
		if (param.empty())
			return;
		if (param[0] == ':')
		{
			cmd.trailing = param;
			return ;
		}
		cmd.arg.push_back(param);
	}
}

std::size_t CommandManager::delimitParameter(const std::string& recv, std::size_t start, std::string& param) const
{
	const char delimiter = ' ';

	std::size_t end = recv.find(delimiter, start);
	while (end != std::string::npos && end - start == 0)
	{
		++start;
		end = recv.find(delimiter, start);
	}
	if (end == std::string::npos || recv[start] == ':')
		end = recv.size();
	param = recv.substr(start, end - start);
	return end;
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
void CommandManager::registerClient(const std::string& recv, const int senderSocket)
{
	Command cmd = parserCmd(recv);
	if (cmd.name != "USER" && cmd.name != "NICK" && cmd.name != "PASS")
		throw NotRegisteredException();
	commands_[cmd.name]->execute(cmd, senderSocket);
}
