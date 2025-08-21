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

	static void createNewTestUser(int fd, const std::string& nickname)
	{
		UserManager::getInstance().addNewUser(fd, dummyAddr(4242));
		UserManager::getInstance().findUser(fd).setNickname(nickname);
		UserManager::getInstance().findUser(fd).passToNextConnectionStep();
		UserManager::getInstance().findUser(fd).passToNextConnectionStep();
		UserManager::getInstance().findUser(fd).passToNextConnectionStep();
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

		UserManagerTester::createNewTestUser(10, "testNick");
		Tester::assertExpectedEqualsActual(10, um.findUserSocket("testNick"));

		UserManagerTester::createNewTestUser(11, "testNick2");
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
		User& u = um.findUser(10);
		u.setNickname("test");
		Tester::assertExpectedEqualsActual(um.findUserNickname(10), std::string("test"));
		u.setUsername("tester");
		Tester::assertExpectedEqualsActual(um.findUserPrefix(10), std::string("test!tester@localhost"));
		u.setNickname("test2");
		Tester::assertExpectedEqualsActual(um.findUserNickname(10), std::string("test2"));
		u.setUsername("tester2");
		Tester::assertExpectedEqualsActual(um.findUserPrefix(10), std::string("test2!tester2@localhost"));

		um.deleteUser(1);
	}
};