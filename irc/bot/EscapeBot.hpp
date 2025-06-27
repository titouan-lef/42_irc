#pragma once

#include <string>

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

	int socketBot_;
	bool running_;

};