
#pragma once

#include "ACommand.hpp"

class PrivmsgCmd : public ACommand
{
public:
    PrivmsgCmd(){}
    ~PrivmsgCmd(){}

    void execute(std::vector<std::string> args, std::string trailing, const User & user) const;
};