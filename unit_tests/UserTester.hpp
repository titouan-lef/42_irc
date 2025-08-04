#pragma once

#include "Tester.hpp"
#include "User.hpp"

#include <iostream>
#include <netinet/in.h>	// struct sockaddr_in
#include <cstring>		// memset()

struct sockaddr_in dummyAddr(int port = 0)
{
	struct sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	return addr;
}

class UserTester
{
public:
	void testerUser()
	{
		testInstance();
		testConnection();
	}

private:

	void testInstance()
	{
		Tester::startTest("User creation");

		User user;

		Tester::assertExpectedEqualsActual(user.getNickname(), std::string("*"));
		Tester::assertTrue(user.getUsername().empty());
		Tester::assertExpectedEqualsActual(DISCONNECTED, user.getStatus());
	}

	void testConnection()
	{
		Tester::startTest("User connection");

		User user;

		Tester::assertExpectedEqualsActual(DISCONNECTED, user.getStatus());

		user.passToNextConnectionStep();
		Tester::assertExpectedEqualsActual(VERIFIED, user.getStatus());
		Tester::assertTrue(user.getStatus() > DISCONNECTED);

		user.setNickname("mister");
		user.passToNextConnectionStep();
		Tester::assertExpectedEqualsActual(IDENTIFIED, user.getStatus());
		Tester::assertTrue(user.getStatus() > VERIFIED);
		Tester::assertExpectedEqualsActual(user.getNickname(), std::string("mister"));

		user.setUsername("kakou");
		user.passToNextConnectionStep();
		Tester::assertExpectedEqualsActual(CONNECTED, user.getStatus());
		Tester::assertTrue(user.getStatus() > IDENTIFIED);
		Tester::assertExpectedEqualsActual(user.getUsername(), std::string("kakou"));
		Tester::assertExpectedEqualsActual(user.getPrefix(), std::string("mister!kakou@localhost"));
	}
};