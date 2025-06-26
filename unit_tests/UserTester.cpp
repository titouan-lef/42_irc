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

static void testInstance()
{
	Tester::startTest("User creation");

	struct sockaddr_in addr = dummyAddr(1234);
	User user(1, addr);

	Tester::assertExpectedEqualsActual(user.getSocket(), 1);
	Tester::assertExpectedEqualsActual(user.getAddress().sin_addr.s_addr, addr.sin_addr.s_addr);
	Tester::assertExpectedEqualsActual(user.getAddress().sin_family, addr.sin_family);
	Tester::assertExpectedEqualsActual(user.getAddress().sin_port, addr.sin_port);
	Tester::assertExpectedEqualsActual(user.getNickname(), std::string(""));
	Tester::assertExpectedEqualsActual(user.getUsername(), std::string(""));
	Tester::assertFalse(user.isConnected());
	Tester::assertFalse(user.isNickRegister());
	Tester::assertFalse(user.isUsernameRegister());
}

/**
 * @brief Function which execute all tests.
 */
void testerUser()
{
	testInstance();
}