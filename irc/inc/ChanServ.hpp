#pragma once

#include "Service.hpp"
#include "Channel.hpp"
#include <vector>

class ChanServ: public Service
{
public:

	static ChanServ& getInstance();

	Channel& findChannelFor(std::string name, User& user);

private:

	ChanServ();
	// ~ChanServ();

	Channel& builtNewChannel(std::string name, User& moderator);
	
	std::vector<Channel> channels_;

};