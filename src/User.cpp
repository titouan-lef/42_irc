#include "User.hpp"

User::User(int clientSocket, struct sockaddr_in clientAddr): clientSocket_(clientSocket), clientAddr_(clientAddr)
{
}

bool User::is(std::string nickname)
{
	return (nickname_ == nickname);
}

std::string User::getNickname()
{
	return nickname_;
}


int User::getSocket()
{
	return (clientSocket_);
}

struct sockaddr_in User::getAddress()
{
	return (clientAddr_);
}