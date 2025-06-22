
#pragma once

#include <iostream>
#include <map>
#include "ACommand.hpp"
#include "User.hpp"

class CommandManager
{
public:
    static CommandManager& getInstance();
    void interpretData(std::string recv, const User & user);
    
private:
    CommandManager();
    ~CommandManager();
    CommandManager(const CommandManager& other);
    CommandManager& operator=(const CommandManager& other);

    std::map<std::string, ACommand*> commands_;
};