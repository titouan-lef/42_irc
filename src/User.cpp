#include "User.hpp"

User::User(int clientSocket, struct sockaddr_in clientAddr): clientSocket_(clientSocket), clientAddr_(clientAddr)
{
}

bool User::is(std::string nickname)
{
	return (nickname_ == nickname);
}

const std::string& User::getNickname() const
{
	return nickname_;
}

const std::string& User::getUsername() const
{
	return username_;
}

const std::string User::getPrefix() const
{
	std::string prefix;

	prefix.append(":");
	prefix.append(nickname_);
	prefix.append("!");
	prefix.append(username_);
	prefix.append("@localhost"); // always ??
	return prefix;
}

int User::getSocket()
{
	return (clientSocket_);
}

struct sockaddr_in User::getAddress()
{
	return (clientAddr_);
}