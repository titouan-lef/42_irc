#pragma once

#include <iostream>

class WeatherBot
{
public:

	static WeatherBot& getInstance();

	int up(const std::string& port, const std::string& password);
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
	void initServerConnection(const std::string& port, const std::string& password);
	void sendCommand(const std::string& cmd) const;
	std::string listenToServer();

	void interpretMsg(const std::string& recvMsg);
	void parseMsg(const std::string& recvMsg, Msg& message);
	std::string extractNickname(const std::string& recvMsg);
	void extractRequest(const std::string& recvMsg, Msg& message);
	void msgExec(Msg& message);
	std::string getReplyMsg(const std::string& msg) const;

	int socketBot_;
	bool running_;
};