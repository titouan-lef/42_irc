#pragma once

#include "ACommand.hpp"

#include <iostream>
#include <vector>

class Channel;
struct Command;

class JoinCmd : public ACommand
{
public:
	JoinCmd();
	~JoinCmd(){}

	void execute(const Command& cmd, const int senderSocket) const;

private:
	bool isPartAllChannelCmd(const std::vector<std::string>& arg) const;
	void partAllChannel(const Command &cmd, const int senderSocket) const;
	void joinChannel(const Command& cmd, const int senderSocket) const;
	std::vector<std::string> definePassword(const Command &cmd) const;
	void connectToAllChannel(const int senderSocket, const std::vector<std::string>& chanNameList, const std::vector<std::string>& passwordList) const;
	void connectToOneChannel(const int senderSocket, const std::string& chanName, const std::vector<std::string>& passwordList, std::size_t& currentIndex) const;
	void connectToNewChannel(const int senderSocket, const std::string& chanName) const;
	void connection(Channel& chan, const int senderSocket, const std::vector<std::string>& passwordList, std::size_t& currentIndex) const;
	const std::string getNextPassword(const Channel& chan, const std::vector<std::string>& passwordList, std::size_t& currentIndex) const;
};