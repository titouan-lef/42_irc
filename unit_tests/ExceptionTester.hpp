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

class ExceptionTester
{
public:
	ExceptionTester(int port, const std::string& password)
	{
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		Tester::assertTrue(sockfd >= 0);
		struct timeval timeout;
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;
		setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

		sockaddr_in server_addr;
		std::memset(&server_addr, 0, sizeof(server_addr));
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(port);
		server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

		int res = connect(sockfd, (sockaddr*)&server_addr, sizeof(server_addr));
		Tester::assertExpectedEqualsActual(res, 0);

		sendCommand("PASS " + password);
		assertReply("Password accepted"); /** @todo complete with good reply */

		sendCommand("NICK testuser");
		assertReply("001"); /** @todo complete with good reply */

		sendCommand("USER testuser 0 * :Real Name");
		assertReply("001"); /** @todo complete with good reply */
	};

	~ExceptionTester()
	{
		close(sockfd);
	};

	void testerException()
	{
		Tester::startTest("Replies from server");

		/** @todo complete with tests */
	};

private:

	void sendCommand(const std::string& cmd)
	{
		std::string fullCmd = cmd + "\r\n";
		/** ssize_t sent = */ send(sockfd, fullCmd.c_str(), fullCmd.length(), 0);
		// Tester::assertExpectedEqualsActual(sent, (ssize_t)fullCmd.length());
	};

std::string readReply()
{
	char buf[1024];
	std::memset(buf, 0, sizeof(buf));
	ssize_t received = recv(sockfd, buf, sizeof(buf) - 1, 0);

	if (received < 0 && (errno == EWOULDBLOCK || errno == EAGAIN))
		Tester::assertExpectedEqualsActual(std::string(""), std::string("time out"));
	else if (received < 0)
		Tester::assertTrue(false);

	return std::string(buf);
}


	void assertReply(const std::string& expected)
	{
		std::string reply = readReply();
		if (!reply.empty())
		Tester::assertExpectedEqualsActual(expected, reply);
	};

	int sockfd;
	// std::string buffer;
};