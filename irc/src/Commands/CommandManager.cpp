#include "CommandManager.hpp"

#include <map>
#include <iostream>
#include <string>
#include <sstream>

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


void CommandManager::interpretData(std::string recv, User& user)
{
	std::string key, token, trailing;
	std::istringstream iss(recv);
	std::vector<std::string> args;

	iss >> key;
	if (commands_.find(key) == commands_.end())
		throw UnknowCmdException(key);
	while (iss >> token)
	{
		if (token[0] == ':')
		{
			trailing = token.substr(1);
			std::string rest;
			std::getline(iss, rest);
			trailing += rest;
			break;
		}
		args.push_back(token);
	}
	if (!user.isUsernameRegister())
		registerClient(key, args, trailing, user);
	else
		commands_[key]->execute(args, trailing, user);
}

void CommandManager::registerClient(std::string key, std::vector<std::string> args, std::string trailing, User & user)
{
	if (!user.isNickRegister())
	{
		if (!user.isConnected())
		{
			if (key != "PASS")
				throw PasswordMismatchException();
			commands_[key]->execute(args, trailing, user);
		}
		else if (key != "NICK")
			throw NotRegisteredException();
		commands_[key]->execute(args, trailing, user);
	}
	else if (key != "USER")
		throw NotRegisteredException();
	commands_[key]->execute(args, trailing, user);
}


