#pragma once

#include "Service.hpp"

#include <vector>
#include <map>

#include "Channel.hpp"

class ChanServ: public Service
{
public:

	static ChanServ& getInstance();

	void receiveCommand(const int senderSocket, const std::string& message);
	
	Channel& findOrCreateChannel(const std::string& name, const int socketUser);
	Channel& findChannel(const std::string& name);
	void deleteChannel(const std::string& name);
	void namesAllChannels(const int socketUser) const;
	void listAllChannels(const int socketUser) const;
	void partAllChannels(const int loggedOutSocket, const std::string& msg = "");
	void sendBroacastToLinkedUsers(const int loggedOutSocket, const std::string& cmdName, const std::vector<std::string>& arg, const std::string& trailing);
	void quit(const int loggedOutSocket, const std::string& msg = "");
	void part(const std::string& channelName, const int loggedOutSocket, const std::string& message = "");
	void kick(const std::string& channelName, const int loggedOutSocket, const int senderSocket, const std::string& message = "");
	bool isValidChannelName(const std::string& channelName) const;
	/** @todo add possibiliti to ask help to ChanServ */

private:
	typedef void (ChanServ::*this_method_t)(int) const;
	typedef std::map<std::string, this_method_t> this_func_map_t;
	
	ChanServ();
	// ~ChanServ();
	std::size_t findChannelIndex(const std::string& channelName) const;
	Channel& builtNewChannel(const std::string& name, const int socketModerator);
	void deleteChannel(const std::size_t index);
	void part(const std::size_t index, const int loggedOutSocket, const std::string& message = "");
	void kick(const std::size_t index, const int loggedOutSocket, const int senderSocket, const std::string& message = "");
	void logOut(const std::size_t index, const int loggedOutSocket);
	
	void sendHelpNotice(const int receiverSocket) const;
	void sendHelpJoinNotice(const int receiverSocket) const;
	void sendHelpPartNotice(const int receiverSocket) const;
	void sendHelpTopicNotice(const int receiverSocket) const;
	void sendHelpPrivmsgNotice(const int receiverSocket) const;
	void sendHelpNamesNotice(const int receiverSocket) const;
	void sendHelpListNotice(const int receiverSocket) const;
	void sendHelpModeNotice(const int receiverSocket) const;
	void sendHelpInviteNotice(const int receiverSocket) const;
	void sendHelpKickNotice(const int receiverSocket) const;

	void sendHelpNoticeReply(const int receiverSocket, const std::string* tabMessage, const std::size_t nbMessage) const;
	void sendNoticeReply(const int receiverSocket, const std::string& message) const;

	std::vector<Channel> channelList_;
	this_func_map_t helpNoticeMap_;

};