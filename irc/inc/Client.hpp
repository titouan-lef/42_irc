#pragma once

#include <iostream>
#include <exception>
#include <arpa/inet.h>		//struct sockaddr_in

class Client
{
public:

	Client();
	// ~Client();

private:

	std::string name;

};