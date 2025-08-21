#include "CommandManager.hpp"

#include "UserManager.hpp"
#include "status.hpp"
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

CommandManager& CommandManager::getInstance()
{
	static CommandManager instance;
	return instance;
}

/**
* @brief Interprets and processes data received from a user.
*/
void CommandManager::interpretData(const std::string& recv, const int senderSocket, const Status& userStatus)
{
	UserManager::getInstance().updateLastActivity(senderSocket);
	Command cmd = parserCmd(recv);
	commands_[cmd.name]->execute(cmd, senderSocket, userStatus);
}

/**
* @brief Parses a raw command string and extracts its components.
*
* @param recv The raw command sent by the client (e.g., "PRIVMSG #channel :Hello all").
*
* @return A 'Command' structure containing the parsed command.
*
* @throws UnknownCmdException.
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
	if (cmdDoNotExist(cmdName))
		throw UnknownCmdException(cmdName);
	return startNextParam;
}

bool CommandManager::cmdDoNotExist(const std::string &cmdName) const
{
	return (cmdName.empty() || commands_.find(cmdName) == commands_.end());
}

void CommandManager::parserCmdElement(const std::string& recv, std::size_t start, Command& cmd) const
{
	cmd.hasTrailing = false;
	while (start != recv.size())
	{
		std::string param;
		start = delimitParameter(recv, start, param);
		if (param.empty())
			return;
		if (param[0] == ':')
		{
			cmd.hasTrailing = true;
			param = param.substr(1);
		}
		cmd.arg.push_back(param);
	}
}

std::size_t CommandManager::delimitParameter(const std::string& recv, std::size_t start, std::string& param) const
{
	const char DELIMITER = ' ';

	std::size_t end = recv.find(DELIMITER, start);
	while (end != std::string::npos && end - start == 0)
	{
		++start;
		end = recv.find(DELIMITER, start);
	}
	if (end == std::string::npos || recv[start] == ':')
		end = recv.size();
	param = recv.substr(start, end - start);
	return end;
}
