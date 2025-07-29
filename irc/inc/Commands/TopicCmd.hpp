#pragma once

#include "ACommand.hpp"

class Channel;
struct Command;

class TopicCmd : public ACommand
{
public:
	TopicCmd(){}
	~TopicCmd(){}

	void execute(const Command& cmd, const int senderSocket) const;

private:
	Channel& checkCmd(const Command& cmd) const;
	bool isGetTopicCmd(const std::size_t nbArg) const;
	void changeTopicName(const Command& cmd, const int senderSocket, Channel& channel) const;
};