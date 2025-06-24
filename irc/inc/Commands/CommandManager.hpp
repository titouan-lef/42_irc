#pragma once

#include <iostream>
#include <map>
#include "ACommand.hpp"
#include "User.hpp"

class CommandManager
{
public:
    static CommandManager& getInstance();
    void interpretData(std::string recv, User & user);
	void registerClient(std::string key, std::vector<std::string> args, std::string trailing, User & user);
    
private:
	CommandManager();
	virtual ~CommandManager();
	CommandManager(const CommandManager& other);
	CommandManager& operator=(const CommandManager& other);

	std::map<std::string, ACommand*> commands_;
};