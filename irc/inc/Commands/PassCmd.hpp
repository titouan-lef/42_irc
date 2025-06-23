#pragma once

#include "ACommand.hpp"

class PassCmd : public ACommand
{
public:
    PassCmd() {}
    ~PassCmd() {}
    
    void execute(std::vector<std::string>args, std::string trailing, User & user) const;
};