#pragma once

#include <iostream>
#include <map>
#include "ACommand.hpp"
#include "User.hpp"

class CommandTester; /** @todo remove before push */

class CommandManager
{
public:
	static CommandManager& getInstance();
	void interpretData(std::string recv, User& user);
	MsgServ::Command parserCmd(const std::string& recv, const User& user);
	void registerClient(MsgServ::Command cmd, User& user);

private:
	CommandManager();
	virtual ~CommandManager();
	CommandManager(const CommandManager& other);
	CommandManager& operator=(const CommandManager& other);

	std::map<std::string, ACommand*> commands_;

	friend class CommandTester ;	/** @todo remove before push */
};