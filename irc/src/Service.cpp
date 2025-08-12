#include "Service.hpp"

std::map<std::string, Service*> Service::serviceMap_;

bool Service::serviceExist(const std::string& serviceName)
{
	return (serviceMap_.find(serviceName) != serviceMap_.end());
}

void Service::sendCommandToService(const std::string& serviceName, const int senderSocket, const std::string& cmd)
{
	if (!serviceExist(serviceName))
		return;
	serviceMap_.at(serviceName)->receiveCommand(senderSocket, cmd);
}

Service::Service(const std::string& serviceName)
{
	if (serviceExist(serviceName))
		return;
	serviceMap_[serviceName] = this;
}
