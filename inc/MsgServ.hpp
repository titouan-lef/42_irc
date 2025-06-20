#pragma once

#include "Service.hpp"
#include "User.hpp"

class MsgServ: public Service
{
public:

	static void sendMsg(int socket, std::string& msg);
	static std::string recvMsg(int socket);
	static int isMsgComplete(std::string& msg);

private:

	MsgServ();
	// ~MsgServ();

};