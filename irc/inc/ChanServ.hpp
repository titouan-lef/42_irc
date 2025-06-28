#pragma once

#include "Service.hpp"
#include "Channel.hpp"
#include <vector>

class ChanServ: public Service
{
public:

	static ChanServ& getInstance();

	Channel& findChannelFor(std::string name, const int socketUser);

private:

	ChanServ();
	// ~ChanServ();

	Channel& builtNewChannel(std::string name, const int socketModerator);
	
	std::vector<Channel> channels_;

};