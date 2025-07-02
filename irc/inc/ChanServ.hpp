#pragma once

#include "Service.hpp"
#include "Channel.hpp"
#include <vector>

class ChanServ: public Service
{
public:

	static ChanServ& getInstance();

	Channel& findChannelFor(std::string name, const int socketUser);
	void quitAllChannel(const std::string& msg, const int socket);

private:

	ChanServ();
	// ~ChanServ();

	Channel& builtNewChannel(std::string name, const int socketModerator);
	void logOut(const size_t index, const std::string& msg, const int socket);

	std::vector<Channel> channels_;

};