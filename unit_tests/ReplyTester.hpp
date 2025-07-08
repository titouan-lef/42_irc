#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <cassert>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "Tester.hpp"

class ReplyTester
{
public:
	ReplyTester(int port, const std::string& password, const std::string& nickname)
	{
		sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
		Tester::assertTrue(sockfd_ >= 0);
		struct timeval timeout;
		timeout.tv_sec = 0;
		timeout.tv_usec = 100000;
		setsockopt(sockfd_, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

		sockaddr_in server_addr;
		std::memset(&server_addr, 0, sizeof(server_addr));
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(port);
		server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

		int res = connect(sockfd_, (sockaddr*)&server_addr, sizeof(server_addr));
		Tester::assertExpectedEqualsActual(res, 0);

		(void)password;
		sendCommand("PASS " + password);
		sendCommand("NICK " + nickname);
		sendCommand("USER " + nickname +" 0 * :Real Name");
		// skipReply();
	};

	~ReplyTester()
	{
		close(sockfd_);
	};

	void sendCommand(const std::string& cmd)
	{
		std::string fullCmd = cmd + "\r\n";
		/** ssize_t sent = */ send(sockfd_, fullCmd.c_str(), fullCmd.length(), MSG_NOSIGNAL);
		// Tester::assertExpectedEqualsActual(sent, (ssize_t)fullCmd.length());
	};

	void assertReply(const std::string& expected)
	{
		std::string reply = readReply(expected);
		if (!reply.empty())
			Tester::assertExpectedEqualsActual(expected + "\r\n", reply);
	};

	void skipReply()
	{
		readReply("");
	};

private:

	std::string readReply(const std::string& expected)
	{
		char buf[1024];
		std::memset(buf, 0, sizeof(buf));
		ssize_t received = recv(sockfd_, buf, sizeof(buf) - 1, 0);

		if (received < 0 && (errno == EWOULDBLOCK || errno == EAGAIN))
			Tester::assertExpectedEqualsActual(std::string(expected), std::string("time out"));
		else if (received < 0)
			Tester::assertTrue(false);

		return std::string(buf);
	}

	int sockfd_;
};