#pragma once

#include "Service.hpp"

#include <iostream>
#include <vector>

#include "command.hpp"

class MsgServTester;/** @todo remove before push */

class MsgServ: public Service
{
public:
	/*struct Command {
		std::string name;
		std::string trailing;
		std::vector<std::string> arg;
	};*/

	static MsgServ& getInstance();

	void recvMsg(int socket, int buffer_index, const std::string& nickname);
	bool isMsgComplete(std::string& msg);
	void sendNumericReply(int socket, const Command& cmd, const std::string& clientTargetNickname);
	void sendMessageReply(int socket, const Command& cmd);

	void addNewBuffer();
	bool isCommandReady(int buffer_index) const;
	int completeMsg(int buffer_index, const std::string &msg);
	std::string getCommand(int buffer_index);
	void removeBuffer(int buffer_index);


private:

	MsgServ();
	MsgServ(const MsgServ& other);
	MsgServ& operator=(const MsgServ& other);

	void sendReply(int socket, const Command& cmd, const std::string& prefix, const std::string& target);
	std::string formatMsg(const Command& cmd, const std::string& prefix, const std::string& target);
	void sendMsg(int socket, std::string& msg);

	std::vector<std::string> buffers_;

	friend class MsgServTester;/** @todo remove before push */
};