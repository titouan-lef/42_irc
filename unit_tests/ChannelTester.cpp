#include "Tester.hpp"
#include "User.hpp"
#include "Channel.hpp"
#include "PasswordDismatchException.hpp"
#include "ChannelIsFullException.hpp"
#include "NoSuchNickException.hpp"

#include <netinet/in.h>	// struct sockaddr_in
#include <cstring>		// memset()

static struct sockaddr_in dummyAddr(int port = 0)
{
	struct sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	return addr;
}

/**
 * @brief Create de Channel check default attributs
 */
static void testInstance()
{
	Tester::startTest("Channel creation");

	User moderator(1, dummyAddr(1234));
	moderator.setNickname("Alice");
	Channel chan("mychannel", moderator);

	Tester::assertExpectedEqualsActual(std::string("mychannel"), chan.getName());
	Tester::assertFalse(chan.isInviteRestricted());
	Tester::assertFalse(chan.isTopicRestricted());
	Tester::assertTrue(chan.isModerator("Alice"));
}

/**
 * @brief Connection with password
 */
static void testPassword()
{

	Tester::startTest("Password protection");

	User bob(2, dummyAddr(5678));
	bob.setNickname("bob");
	User alice(1, dummyAddr(1234));
	alice.setNickname("alice");
	Channel chan("secure", alice);

	Tester::assertDoNotThrows([&]() {chan.connectUser(bob, "wrongpass");});

	chan.setPassword("secret");

	Tester::assertThrows<PasswordDismatchException>([&]() {chan.connectUser(bob);});
	Tester::assertThrows<PasswordDismatchException>([&]() {chan.connectUser(bob, "wrongpass");});

	chan.disablePassword();

	Tester::assertDoNotThrows([&]() {chan.connectUser(bob, "wrongpass");});

}

/**
 * @brief User limit
 */
static void testUserLimit()
{

	Tester::startTest("User limit");

	User mod(1, dummyAddr(1001));
	mod.setNickname("mod");
	User u1(2, dummyAddr(1002));
	u1.setNickname("u1");
	User u2(3, dummyAddr(1003));
	u1.setNickname("u1");
	Channel chan("limited", mod);
	chan.setLimitNbUsers(2);

	Tester::assertDoNotThrows([&]() {chan.connectUser(u1);});
	Tester::assertThrows<ChannelIsFullException>([&]() {chan.connectUser(u2);});

}

/**
 * @brief Disconnection
 */
static void testUserDisconnection()
{
	Tester::startTest("User disconnection");

	User mod(5, dummyAddr(9999));
	mod.setNickname("mod");
	Channel chan("disc", mod);

	Tester::assertThrows<PasswordDismatchException>([&]() {chan.connectUser(mod, "mod");});

}

/**
 * @brief Function which execute all tests.
 */
int main()
{

	testInstance();
	testPassword();
	testUserLimit();
	testUserDisconnection();

	/** @todo defines other tests */
}
