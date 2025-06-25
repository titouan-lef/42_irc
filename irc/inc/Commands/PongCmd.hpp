#pragma once

#include "ACommand.hpp"

class PongCmd : public ACommand
{
public:
    PongCmd(){}
    ~PongCmd(){}
    
    void execute(MsgServ::Command cmd, User & user) const;
};