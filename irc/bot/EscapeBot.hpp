#pragma once

#include <string>
#include "Msg.hpp"

class EscapeBot
{
public:

	static EscapeBot& getInstance();

	int up(std::string port,std::string password);
	void stop();

private:

	EscapeBot();
	~EscapeBot() {};

	void run();
	void initServerConnection(std::string port,std::string password);
	int parsePort(const std::string& port);
	void sendCommand(const std::string& cmd);
	std::string listenToServer();

	void interpretMsg(const std::string& recvMsg);
	void parseMsg(const std::string& recvMsg, Msg& message);
	std::string extractNickname(const std::string& recvMsg);
	std::string extractServer(const std::string& recvMsg);
	void extractRequest(const std::string& recvMsg, Msg& message);
	void msgExec(Msg& message);

	int socketBot_;
	bool running_;

};