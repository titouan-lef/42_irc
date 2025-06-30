#pragma once

#include <iostream>
#include <vector>

#include "Service.hpp"
#include "MsgServ.hpp"

class MsgServTester;/** @todo remove before push */

class MsgServ: public Service
{
public:
	struct Command {
		std::string name;
		std::string trailing;
		std::vector<std::string> arg;
	};
	static std::string recvMsg(int socket, const std::string& nickname);
	static bool isMsgComplete(std::string& msg);
	static void sendNumericReply(int socket, const MsgServ::Command& cmd, const std::string& clientTargetNickname);
	static void sendMessageReply(int socket, const MsgServ::Command& cmd);

private:

	MsgServ();
	// ~MsgServ();

	static void sendReply(int socket, const MsgServ::Command& cmd, const std::string& prefix, const std::string& target);
	static std::string formatMsg(const MsgServ::Command& cmd, const std::string& prefix, const std::string& target);
	static void sendMsg(int socket, std::string& msg);

	friend class MsgServTester;/** @todo remove before push */
};