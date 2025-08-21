#include "Tester.hpp"
#include "ChannelTester.hpp"
#include "MsgServTester.hpp"
#include "UserTester.hpp"
#include "ReplyTester.hpp"
#include "CommandTester.hpp"
#include "UserManagerTester.hpp"
#include "ModeCmdTester.hpp"
#include "TopicCmdTester.hpp"
#include "InviteCmdTester.hpp"
#include "PassCmdTester.hpp"
#include "NickCmdTester.hpp"
#include "UserCmdTester.hpp"
#include "JoinCmdTester.hpp"
#include "PartCmdTester.hpp"
#include "QuitCmdTester.hpp"
#include "KickCmdTester.hpp"
#include "PrivMsgTester.hpp"
#include "ListCmdTester.hpp"

int main()
{
	UserManager::getInstance().addNewUser(0, dummyAddr(4242)); //simulate server at user 0
	{
		UserTester test;
		test.testerUser();
	}
	{
		UserManagerTester test;
		test.testerUserManager();
	}
	{
		ChannelTester test;
		test.testerChannel();
	}
	{
		MsgServTester test;
		test.testerMsgServ();
	}
	{
		CommandTester test;
		test.testerCommand();
	}
	{
		PassCmdTester test;
		test.testerPassCmd();
	}
	{
		NickCmdTester test;
		test.testerNickCmd();
	}
	{
		UserCmdTester test;
		test.testerUserCmd();
	}
	{
		JoinCmdTester test;
		test.testerJoinCmd();
	}
	{
		PartCmdTester test;
		test.testerPartCmd();
	}
	{
		QuitCmdTester test;
		test.testerQuitCmd();
	}
	{
		TopicCmdTester test;
		test.testerTopicCmd();
	}
	{
		InviteCmdTester test;
		test.testerInviteCmd();
	}
	{
		KickCmdTester test;
		test.testerKickCmd();
	}
	{
		ModeCmdTester test;
		test.testerModeCmd();
	}
	{
		PrivMsgTester test;
		test.testerPrivMsg();
	}
	{
		ListCmdTester test;
		test.testerListCmd();
	}
	Tester::printResults();
}
