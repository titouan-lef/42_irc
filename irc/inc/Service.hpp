#pragma once

#include "Client.hpp"

#include <iostream>
#include <map>

class Service: public Client
{
public:
	static bool serviceExist(const std::string& serviceName);
	static void sendCommandToService(const std::string& serviceName, const int senderSocket, const std::string& cmd);

protected:
	Service(const std::string& serviceName);

	virtual void receiveCommand(const int senderSocket, const std::string& cmd) = 0;

private:
	static std::map<std::string, Service*> serviceMap_;
};