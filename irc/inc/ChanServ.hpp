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
	void partAllChannels(const int loggedOutSocket, const std::string& msg = "");
	void sendBroacastToLinkedUsers(const int loggedOutSocket, const Command& cmd);
	void quit(const int loggedOutSocket, const std::string& msg = "");
	void part(const std::string& channelName, const int loggedOutSocket, const std::string& message = "");
	void kick(const std::string& channelName, const int loggedOutSocket, const int senderSocket, const std::string& message = "");
	bool isValidChannelName(const std::string& channelName) const;
	/** @todo add possibiliti to ask help to ChanServ */

private:

	ChanServ();
	// ~ChanServ();
	size_t findChannelIndex(std::string channelName);
	Channel& builtNewChannel(std::string name, const int socketModerator);
	void deleteChannel(const int index);
	void part(const size_t index, const int loggedOutSocket, const std::string& message = "");
	void kick(const size_t index, const int loggedOutSocket, const int senderSocket, const std::string& message = "");
	void logOut(const size_t index, const int loggedOutSocket);

	std::vector<Channel> channels_;

};