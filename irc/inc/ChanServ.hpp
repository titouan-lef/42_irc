#pragma once

#include "Service.hpp"

#include <vector>

#include "Channel.hpp"

class ChanServ: public Service
{
public:

	static ChanServ& getInstance();

	void receiveCommand(const int senderSocket, const std::string &message);
	
	Channel& findOrCreateChannel(std::string name, const int socketUser);
	Channel& findChannel(std::string name);
	void deleteChannel(const std::string& name);
	void namesAllChannels(const int socketUser);
	void listAllChannels(const int socketUser);
	void partAllChannels(const int loggedOutSocket, const std::string& msg = "");
	void sendBroacastToLinkedUsers(const int loggedOutSocket, const std::string &cmdName, const std::vector<std::string> &arg, const std::string &trailing);
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
	
	void sendHelpNotice(const int recieverSocket);
	void sendHelpJoinNotice(const int recieverSocket);
	void sendHelpPartNotice(const int recieverSocket);
	void sendHelpTopicNotice(const int recieverSocket);
	void sendHelpPrivmsgNotice(const int recieverSocket);
	void sendHelpNamesNotice(const int recieverSocket);
	void sendHelpListNotice(const int recieverSocket);
	void sendHelpModeNotice(const int recieverSocket);
	void sendHelpInviteNotice(const int recieverSocket);
	void sendHelpKickNotice(const int recieverSocket);

	std::vector<Channel> channels_;

};