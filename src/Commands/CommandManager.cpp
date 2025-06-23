
#include <map>
#include <iostream>
#include <string>
#include <sstream>
#include "CommandManager.hpp"
#include "InviteCmd.hpp"
#include "JoinCmd.hpp"
#include "KickCmd.hpp"
#include "ModeCmd.hpp"
#include "NickCmd.hpp"
#include "PartCmd.hpp"
#include "PingCmd.hpp"
#include "PongCmd.hpp"
#include "PrivmsgCmd.hpp"
#include "QuitCmd.hpp"
#include "TopicCmd.hpp"
#include "UserCmd.hpp"
#include "UnknowCmdException.hpp"

CommandManager::CommandManager() {
    commands_["INVITE"] = new InviteCmd();
    commands_["JOIN"] = new JoinCmd();
    commands_["KICK"] = new KickCmd();
    commands_["MODE"] = new ModeCmd();
    commands_["NICK"] = new NickCmd();
    commands_["PART"] = new PartCmd();
    commands_["PING"] = new PingCmd();
    commands_["PONG"] = new PongCmd();
    commands_["PRIVMSG"] = new PrivmsgCmd();
    commands_["QUIT"] = new QuitCmd();
    commands_["TOPIC"] = new TopicCmd();
    commands_["USER"] = new UserCmd();
}

CommandManager::~CommandManager() {
    delete commands_["INVITE"];
    delete commands_["JOIN"];
    delete commands_["KICK"];
    delete commands_["MODE"];
    delete commands_["NICK"];
    delete commands_["PART"];
    delete commands_["PING"];
    delete commands_["PONG"];
    delete commands_["PRIVMSG"];
    delete commands_["QUIT"];
    delete commands_["TOPIC"];
    delete commands_["USER"];
}

CommandManager& CommandManager::getInstance()
{
    static CommandManager instance;
    return instance;
}

void CommandManager::interpretData(std::string recv, const User & user)
{
    std::string key, token, trailing;
    std::istringstream iss(recv);
    std::vector<std::string> args;

    iss >> key;
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
        else
            args.push_back(token);
    }
    std::map<std::string, ACommand*>::iterator it = commands_.find(key);
    if (it != commands_.end())
        commands_[key]->execute(args, trailing, user);
    else
        throw UnknowCmdException(key);
}
