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

		struct sockaddr_in addr = dummyAddr(1234);
		User user(1, addr);

		Tester::assertExpectedEqualsActual(user.getSocket(), 1);
		Tester::assertExpectedEqualsActual(user.getAddress().sin_addr.s_addr, addr.sin_addr.s_addr);
		Tester::assertExpectedEqualsActual(user.getAddress().sin_family, addr.sin_family);
		Tester::assertExpectedEqualsActual(user.getAddress().sin_port, addr.sin_port);
		Tester::assertTrue(user.getNickname().empty());
		Tester::assertTrue(user.getUsername().empty());
		Tester::assertFalse(user.isConnected());
		Tester::assertFalse(user.isNickRegister());
		Tester::assertFalse(user.isUsernameRegister());
	}

	void testConnection()
	{
		Tester::startTest("User connection");

		struct sockaddr_in addr = dummyAddr(1234);
		User user(1, addr);

		user.validConnection();
		Tester::assertTrue(user.isConnected());
		user.setNickname("mister");
		Tester::assertExpectedEqualsActual(user.getNickname(), std::string("mister"));
		Tester::assertTrue(user.isNickRegister());
		user.setUsername("kakou");
		Tester::assertExpectedEqualsActual(user.getUsername(), std::string("kakou"));
		Tester::assertTrue(user.isUsernameRegister());
		Tester::assertExpectedEqualsActual(user.getPrefix(), std::string(":mister!kakou@localhost"));
	}
};