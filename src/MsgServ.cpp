#include <cerrno>
#include "MsgServ.hpp"
#include "ClientDisconnectedException.hpp"
#include "ReiceveException.hpp"
#include "SendException.hpp"

void MsgServ::sendMsg(int socket, std::string& msg)
{
	std::string nickname = "tmpNickname"; /** @todo find right nickname */
	if (msg.empty())
		return;
	ssize_t sent = send(socket, msg.c_str(), msg.size(), 0);
	if (sent == -1 && errno != EAGAIN)
		throw SendException(nickname);
	else if (sent > 0)
		msg.erase(0, sent);
}

std::string MsgServ::recvMsg(int socket)
{
	std::string nickname = "tmpNickname"; /** @todo find right nickname */
	char buffer[1024];
	int bytes_received = recv(socket, buffer, sizeof(buffer) - 1, 0);
	if (bytes_received < 0)
		throw ReiceveException(nickname);
	else if (bytes_received == 0) // Connection closed by client
		throw ClientDisconnectedException(nickname);
	buffer[bytes_received] = '\0';
	return buffer;
}

int MsgServ::isMsgComplete(std::string& msg)
{
	return (msg.find("\r\n") != std::string::npos);
}

