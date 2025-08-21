#include "WeatherBot.hpp"

#include <cstring>
#include <unistd.h>
#include <cstdlib>
#include <vector>
#include <fstream>

#include "connexionUtils.hpp"


WeatherBot& WeatherBot::getInstance()
{
	static WeatherBot instance;
	return instance;
}

int WeatherBot::up(const std::string& port, const std::string& password)
{
	try
	{
		initServerConnection(port, password);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		std::cout << std::endl << "Shutting down WeatherBot..." << std::endl;
		return 1;
	}
	run();
	return 0;
}

void WeatherBot::sendCommand(const std::string& cmd) const
{
	std::string fullCmd = cmd + "\r\n";
	ssize_t sent = send(socketBot_, fullCmd.c_str(), fullCmd.length(), 0);
	if (sent < 0)
		std::cerr << "Error: Impossible to send the message" << std::endl;
	else if (size_t(sent) < fullCmd.size())
		std::cerr << "Error, the message sent is incomplete" << std::endl;
}

WeatherBot::WeatherBot(): socketBot_(-1), running_(false) {}

void WeatherBot::run()
{
	running_ = true;
	while (running_)
	{
		std::string recvMsg = listenToServer();
		interpretMsg(recvMsg);
	}
}

void WeatherBot::initServerConnection(const std::string& port, const std::string& password)
{
	int portNumber = parsePort(port);
	socketBot_ = createSocket();
	defineSocketOption(socketBot_);
	struct sockaddr_in serverAddr = createAddress(portNumber);

	int res = connect(socketBot_, (sockaddr*)&serverAddr, sizeof(serverAddr)); // connect to server address
	if (res < 0)
	{
		close(socketBot_);
		throw std::runtime_error("Error: impossible to connect to the server");
	}

	sendCommand("PASS " + password);
	sendCommand("NICK WthBot");
	sendCommand("USER weatherBot 0 * :Master of the universe");
}

std::string WeatherBot::listenToServer()
{
	char buf[512];
	std::memset(buf, 0, sizeof(buf));
	ssize_t received = recv(socketBot_, buf, sizeof(buf) - 1, 0);
	if (received < 0)
	{
		if (running_ == true)
		{
			std::cerr << "Error: problem with server communication." << std::endl;
			stop();
		}
	}
	if (received == 0) // Connection closed by server
	{
		std::cout << "Server disconnected." << std::endl;
		stop();
	}
	return buf;
}

void WeatherBot::stop()
{
	if (socketBot_ >= 0)
	{
		close(socketBot_);
		socketBot_ = -1;
	}
	running_ = false;
	std::cout << std::endl << "Shutting down WeatherBot..." << std::endl;
}

/********************************************* interpret msg ************************************************/
void WeatherBot::interpretMsg(const std::string& recvMsg)
{
	Msg message;
	WeatherBot::parseMsg(recvMsg, message);
	WeatherBot::msgExec(message);
}

void WeatherBot::WeatherBot::parseMsg(const std::string& recvMsg, Msg& message)
{
	if (recvMsg.empty())
		return ;
	message.nickSender_ = extractNickname(recvMsg);
	if (message.nickSender_.empty())
		return ;
	extractRequest(recvMsg, message);
}

std::string WeatherBot::extractNickname(const std::string& recvMsg)
{
	std::string nickname;
	size_t start = recvMsg.find(':');
	if (start == std::string::npos || start == recvMsg.size())
		return nickname;
	size_t end = recvMsg.find('!');
	if (end == std::string::npos || end == recvMsg.size())
		return recvMsg.substr(start + 1);
	nickname = recvMsg.substr(start + 1, end -1);
	return nickname;
}

void WeatherBot::extractRequest(const std::string& recvMsg, Msg& message)
{
	const char delimiter = ' ';
	size_t start = recvMsg.find(delimiter, 0);
	if (start == std::string::npos)
		return ;
	++start;
	size_t end = recvMsg.find(delimiter, start);
	if (end == std::string::npos)
		return ;
	message.cmd_ = recvMsg.substr(start, end - start);
	if (end == recvMsg.size())
		return ;
	++end;
	message.restOfMsg_ = recvMsg.substr(end, recvMsg.size() - end);
}

void WeatherBot::msgExec(Msg& message)
{
	if (message.cmd_ == "433" || message.cmd_ == "451" || message.cmd_ == "464") // 433 : ERR_NICKNAMEINUSE , 451 : ERR_NOTREGISTERED, 464 : ERR_PASSWDMISMATCH
	{
		std::cerr << "Impossible to connect to the server" << std::endl;
		stop();
	}
	else if (message.cmd_ == "PING")
		sendCommand("PONG " + message.restOfMsg_);
	else if (message.cmd_ == "PRIVMSG")
	{
		std::string replyMsg;
		replyMsg = getReplyMsg(message.restOfMsg_);
		sendCommand("PRIVMSG "+ message.nickSender_ + " :" + replyMsg);
	}
}

/********************************************* Get Weather ************************************************/
bool checkInputSafety(const std::string& city)
{
	for (std::string::const_iterator it = city.begin(); it != city.end(); ++it)
	{
		if (!std::isalpha(static_cast<unsigned char>(*it)))
			return false;
	}
	return true;
}

std::string getText()
{
	std::ifstream weather;
	weather.open(".txt");
	std::string data;
	std::getline(weather, data);
	weather.close();
	return (data);
}

std::string getWeather(const std::string& city)
{
	std::string cityName = city.substr(0, city.size() - 2);
	if (!checkInputSafety(cityName))
		return ("Try with a valid name of city");
	const std::string cmd = ("/bin/curl -s \"https://wttr.in/" + cityName + "?format=3\" > .txt"); //-s silent function curl
	int cr = system(cmd.c_str());
	if (cr != 0)
		return ("An error has occured, the weather is unaccessible");
	std::string weather;
	weather = getText();
	cr = unlink(".txt");
	if (cr != 0)
		return ("An error has occured, the weather is unaccessible");
	return (weather);
}

std::vector<std::string> splitOnSpace(std::string msg)
{
	const char DELIMITER = ' ';

	std::vector<std::string> targetList;
	std::string target;
	std::size_t start = 0;
	std::size_t end = msg.find(DELIMITER);

	while (end != std::string::npos)
	{
		target = msg.substr(start, end - start);
		targetList.push_back(target);
		start = end + 1;
		end = msg.find(DELIMITER, start);
	}
	target = msg.substr(start);
	targetList.push_back(target);
	return targetList;
}

std::string WeatherBot::getReplyMsg(const std::string& msg) const
{
	std::vector<std::string> args = splitOnSpace(msg);
	if (args.size() != 3)
		return "Please try with WEATHER one city name.";
	if (args[1] == ":WEATHER")
		return getWeather(args[2]);
	return "Please try with WEATHER one city name.";
}