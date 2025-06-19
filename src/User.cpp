#include "User.hpp"

User::User(int clientSocket, struct sockaddr_in clientAddr): clientSocket_(clientSocket), clientAddr_(clientAddr)
{
}

const std::string& User::receiveMsg()
{
	buffer_.clear();
	char buffer[1024];
	int bytes_received = recv(clientSocket_, buffer, sizeof(buffer) - 1, 0);
	if (bytes_received < 0)
		throw ErrorReiceve();
	else if (bytes_received == 0)	// Connection closed by client
		throw ClientDisconnected();
	buffer[bytes_received] = '\0';
	buffer_.append(buffer);
	return (buffer_);
}

void User::sendMsg(const std::string& msg)
{
	send(clientSocket_, msg.c_str(), msg.length(), 0);
}

void User::privateMsg(User* target , const std::string& msg) // ptr on clien on string(nickname)
{
	target->sendMsg(msg);
}

int User::getSocket()
{
	return (clientSocket_);
}

struct sockaddr_in User::getAddress()
{
	return (clientAddr_);
}

const char* User::ErrorReiceve::what() const throw()
{
	return ("Error: recv failed");
}

const char* User::ClientDisconnected::what() const throw()
{
	return ("Error: client disconnected");
}