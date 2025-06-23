
#pragma once

#include "ACommand.hpp"

class NickCmd : public ACommand
{
public:
    NickCmd(){}
    ~NickCmd(){}

    void execute(std::vector<std::string>args, std::string trailing, const User & user) const;
};