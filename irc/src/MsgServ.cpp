#include "MsgServ.hpp"

#include <cerrno>

#include "ClientDisconnectedException.hpp"
#include "ReiceveException.hpp"
#include "SendException.hpp"

/**
 * @brief Receives a message from the specified socket.
 *
 * @param socket The socket file descriptor to receive data from.
 *
 * @return The received message as a std::string.
 *
 * @throws ReiceveException if an error occurs during receiving.
 * @throws ClientDisconnectedException if the client has closed the connection.
 *
 * @note Currently uses a temporary nickname placeholder for exception reporting.
 */
std::string MsgServ::recvMsg(int socket, const std::string& nickname)
{
	char buffer[1024];
	int bytes_received = recv(socket, buffer, sizeof(buffer) - 1, 0);
	if (bytes_received < 0)
		throw ReiceveException(socket, nickname);
	if (bytes_received == 0) // Connection closed by client
		throw ClientDisconnectedException(socket, nickname);
	buffer[bytes_received] = '\0';
	return buffer;
}

/**
 * @brief Checks if the message contains the end-of-message delimiter "\r\n".
 *
 * @param msg The message string to check.
 *
 * @return true if the message contains "\r\n", false otherwise.
 */
bool MsgServ::isMsgComplete(std::string& msg)
{
	return (msg.find("\r\n") != std::string::npos);
}

void MsgServ::sendNumericReply(int socket, const MsgServ::Command& cmd, const std::string& clientTargetNickname)
{
	sendReply(socket, cmd, "ircServer", clientTargetNickname);/** @todo ircServer ? server? other? */
}

void MsgServ::sendMessageReply(int socket, const MsgServ::Command& cmd, const std::string& prefix)
{
	sendReply(socket, cmd, prefix, "");
}

void MsgServ::sendReply(int socket, const MsgServ::Command& cmd, const std::string& prefix, const std::string& target)
{
	std::string reply = formatMsg(cmd, prefix, target);
	sendMsg(socket, reply);/** @todo manage exception */
}

std::string MsgServ::formatMsg(const MsgServ::Command& cmd, const std::string& prefix, const std::string& target)
{
	std::string reply = ":" + prefix + " " + cmd.name;
	if (target != "")
		reply += " " + target;
	for (std::size_t i = 0; i < cmd.arg.size(); ++i)
		reply += " " + cmd.arg[i];
	if (cmd.trailing != "")
		reply += " :" + cmd.trailing; 
	reply += "\r\n";
	return reply;
}

/**
 * @brief Sends a message over the given socket.
 *
 * @param socket The socket file descriptor to send the message through.
 * @param msg The message to send; the sent part will be erased from this string.
 *
 * @throws SendException if sending fails for reasons other than EAGAIN.
 *
 * @note Currently uses a temporary nickname placeholder for error reporting.
 */
void MsgServ::sendMsg(int socket, std::string& msg)
{
	/** std::string nickname = "tmpNickname"; @todo find right nickname or simply get a reference to user so that we can access nickname and socket ??*/
	if (msg.empty())
		return;
	ssize_t sent = send(socket, msg.c_str(), msg.size(), 0);
	if (sent == -1 && errno != EAGAIN)
		throw std::runtime_error("Error: cannot send message");
	if (sent > 0)
		msg.erase(0, sent);
}
