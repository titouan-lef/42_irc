#pragma once

#include <iostream>

class WeatherBot
{
public:

	static WeatherBot& getInstance();

	int up(std::string port,std::string password);
	void stop();

private:

	struct Msg {
		std::string nickSender_;
		std::string cmd_;
		std::string restOfMsg_;
	};

	WeatherBot();
	~WeatherBot() {};

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
	std::string getReplyMsg(std::string msg);

	int socketBot_;
	bool running_;
};