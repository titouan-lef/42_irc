#pragma once

#include <vector>

#include "Service.hpp"

class MsgServTester;

class MsgServ: public Service
{
public:
	struct Command {
		std::string name;
		std::string trailing;
		std::vector<std::string> arg;
	};

	static void sendMsg(int socket, std::string& msg);
	static std::string recvMsg(int socket);
	static bool isMsgComplete(std::string& msg);
	static const std::string getNumericReply(MsgServ::Command& cmd, const int code, const std::string& clientTargetNickname);
	static const std::string getMessageReply(const MsgServ::Command& cmd, const std::string& clientNickname);

private:

	MsgServ();
	// ~MsgServ();

	static const std::string getReply(const MsgServ::Command& cmd, const std::string& source, const std::string& target);

	friend class MsgServTesterl;
};