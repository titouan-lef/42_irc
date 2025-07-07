#pragma once

#include "Service.hpp"
#include "Channel.hpp"
#include <vector>

class ChanServ: public Service
{
public:

	static ChanServ& getInstance();

	Channel& findOrCreateChannel(std::string name, const int socketUser);
	Channel& findChannel(std::string name);
	void deleteChannel(const std::string& name);
	void namesAllChannels(const int socketUser);
	void listAllChannels(const int socketUser);
	void quitAllChannels(const int logedOutSocket, const std::string& msg = "");
	void logOut(const std::string& channelName, const Command& cmd, const int logedOutsocket);

private:

	ChanServ();
	// ~ChanServ();
	void deleteChannel(const int index);
	Channel& builtNewChannel(std::string name, const int socketModerator);
	void logOut(const size_t index, const Command& cmd, const int logedOutSocket);

	std::vector<Channel> channels_;

};