#pragma once

#include "Tester.hpp"
#include "UserManager.hpp"
#include "UserTester.hpp"  // dummyAddr()

#include <unistd.h>

class UserManagerTester
{
public:
	void testerUserManager()
	{
		testSingletonInstance();
		testFindUserSocket();
		testGetUserNicknameAndPrefix();
	}

private:
	void testSingletonInstance()
	{
		Tester::startTest("UserManager Singleton instance");

		UserManager& um1 = UserManager::getInstance();
		UserManager& um2 = UserManager::getInstance();

		Tester::assertTrue(&um1 == &um2);
	}

	void testFindUserSocket()
	{
		Tester::startTest("UserManager Find user socket");

		UserManager& um = UserManager::getInstance();

		um.addNewUser(10, dummyAddr(4242));
		um.getUser(10).setNickname("testNick");
		Tester::assertExpectedEqualsActual(10, um.findUserSocket("testNick"));

		um.addNewUser(11, dummyAddr(4243));
		um.getUser(11).setNickname("testNick2");
		Tester::assertExpectedEqualsActual(10, um.findUserSocket("testNick"));
		Tester::assertExpectedEqualsActual(11, um.findUserSocket("testNick2"));
		
		um.deleteUser(1);
		um.deleteUser(1);
	}

	void testGetUserNicknameAndPrefix()
	{
		Tester::startTest("UserManager Get nickname and prefix");

		UserManager& um = UserManager::getInstance();

		um.addNewUser(10, dummyAddr(4242));
		User& u = um.getUser(10);
		u.setNickname("test");
		Tester::assertExpectedEqualsActual(um.getUserNickname(10), std::string("test"));
		u.setUsername("tester");
		Tester::assertExpectedEqualsActual(um.getUserPrefix(10), std::string("test!tester@localhost"));
		u.setNickname("test2");
		Tester::assertExpectedEqualsActual(um.getUserNickname(10), std::string("test2"));
		u.setUsername("tester2");
		Tester::assertExpectedEqualsActual(um.getUserPrefix(10), std::string("test2!tester2@localhost"));

		um.deleteUser(1);
	}
};