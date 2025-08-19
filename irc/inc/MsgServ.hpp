#pragma once

#include "Service.hpp"

#include <iostream>
#include <vector>

class MsgServTester;/** @todo remove before push */

struct Command;

class MsgServ: public Service
{
public:
	static MsgServ& getInstance();
	void recvMsg(int receiverSocket, int buffer_index);

	void sendNumericReply(const int receiverSocket, const int code, const std::string& trailing) const;
	void sendNumericReply(const int receiverSocket, const int code, const std::vector<std::string>& arg) const;
	void sendNumericReply(const int receiverSocket, const int code, const std::vector<std::string>& arg, const std::string& trailing) const;

	void sendMessageReply(const int senderSocket, const int receiverSocket, const Command& cmd) const;
	void sendMessageReply(const int senderSocket, const int receiverSocket, const std::string& commandId, const std::string& trailing) const;
	void sendMessageReply(const int senderSocket, const int receiverSocket, const std::string& commandId, const std::vector<std::string>& arg) const;
	void sendMessageReply(const int senderSocket, const int receiverSocket, const std::string& commandId, const std::vector<std::string>& arg,
		const std::string& trailing) const;

	void sendNoticeReply(const std::string serviceName, const int receiverSocket, const std::string &trailing);

	void addNewBuffer();
	bool isCommandReady(int buffer_index) const;
	std::string getCommand(int buffer_index);
	void removeBuffer(int buffer_index);

private:
	struct Message {
		int receiverSocket;
		std::string senderPrefix;
		std::string commandId;
		std::vector<std::string> arg;
		bool hasTrailing;
		std::string trailing;
	};

	MsgServ();

	void receiveCommand(const int senderSocket, const std::string& cmd);
	void sendNumericReply(const int receiverSocket, const int code, const std::vector<std::string>* arg = NULL,
		const std::string* trailing = NULL) const;
	void sendMessageReply(const int senderSocket, const int receiverSocket, const std::string &commandId,
		const std::vector<std::string>* arg = NULL, const std::string* trailing = NULL) const;
	Message initMessage(const int receiverSocket, const std::string& senderPrefix, const std::string& commandId, const std::string* trailing) const;
	void sendReply(const Message& msg) const;
	std::string formatMsg(const Message &msg) const;
	void sendMsg(int socket, std::string& msg) const;
	const std::string convertCodeToCommandId(const int code) const;
	bool completeMsg(int buffer_index, const std::string &msg);


	static const std::size_t BUFFER_SIZE = 512;
	std::vector<std::string> buffers_;

	friend class MsgServTester;/** @todo remove before push */
};