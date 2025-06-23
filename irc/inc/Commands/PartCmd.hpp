#pragma once

#include "ACommand.hpp"

class PartCmd : public ACommand
{
public:
    PartCmd() {}
    ~PartCmd() {}
    
    void execute(std::vector<std::string>args, std::string trailing, User & user) const;
};