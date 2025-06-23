
#pragma once

#include "ACommand.hpp"

class InviteCmd : public ACommand
{
public:
    InviteCmd(){}
    ~InviteCmd(){}

    void execute(std::vector<std::string>args, std::string trailing, const User & user) const;
};