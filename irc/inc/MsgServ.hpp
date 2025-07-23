#pragma once

#include "Service.hpp"

#include <iostream>
#include <vector>

class MsgServTester;/** @todo remove before push */

class MsgServ: public Service
{
public:
	static MsgServ& getInstance();

	void recvMsg(int socket, int buffer_index);
	bool isMsgComplete(std::string& msg);

	void sendNumericReply(const int receiverSocket, const int code, const std::string& trailing = "");
	void sendNumericReply(const int receiverSocket, const int code, const std::vector<std::string>& arg, const std::string& trailing = "");

	void sendMessageReply(const int senderSocket, const int receiverSocket, const std::string& cmdName, const std::string& trailing = "");
	void sendMessageReply(const int senderSocket, const int receiverSocket, const std::string& cmdName, const std::vector<std::string>& arg,
		const std::string& trailing = "");

	void sendNoticeReply(const std::string serviceName, const int receiverSocket, const std::string &trailing);

	void addNewBuffer();
	bool isCommandReady(int buffer_index) const;
	int completeMsg(int buffer_index, const std::string &msg);
	std::string getCommand(int buffer_index);
	void removeBuffer(int buffer_index);


private:
	struct Message {
		int receiverSocket;
		std::string senderPrefix;
		std::string commandId;
		std::vector<std::string> arg;
		std::string trailing;
	};

	MsgServ();
	MsgServ(const MsgServ& other);
	MsgServ& operator=(const MsgServ& other);

	void sendReply(const Message& msg);
	std::string formatMsg(const Message &msg);
	void sendMsg(int socket, std::string& msg);
	const std::string convertCodeToCommandId(const int code);

	static const std::size_t BUFFER_SIZE = 512;
	std::vector<std::string> buffers_;

	friend class MsgServTester;/** @todo remove before push */
};