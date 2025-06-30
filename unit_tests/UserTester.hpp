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
		Tester::assertFalse(user.isConnected());
		Tester::assertFalse(user.isNickRegister());
		Tester::assertFalse(user.isUsernameRegister());
	}

	void testConnection()
	{
		Tester::startTest("User connection");

		User user;

		user.validConnection();
		Tester::assertTrue(user.isConnected());
		user.setNickname("mister");
		Tester::assertExpectedEqualsActual(user.getNickname(), std::string("mister"));
		Tester::assertTrue(user.isNickRegister());
		user.setUsername("kakou");
		Tester::assertExpectedEqualsActual(user.getUsername(), std::string("kakou"));
		Tester::assertTrue(user.isUsernameRegister());
		Tester::assertExpectedEqualsActual(user.getPrefix(), std::string("mister!kakou@localhost"));
	}
};