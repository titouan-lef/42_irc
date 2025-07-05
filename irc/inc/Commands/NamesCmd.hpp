#pragma once

#include "ACommand.hpp"

class NamesCmd : public ACommand
{
public:
	NamesCmd(){}
	~NamesCmd(){}

	void execute(const MsgServ::Command& cmd, const int socket) const;
	std::vector<std::string> split(const std::string& str, char delimiter) const;

};