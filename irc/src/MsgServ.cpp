#include "MsgServ.hpp"

#include <cerrno>

#include "ClientDisconnectedException.hpp"
#include "ReiceveException.hpp"
#include "UserManager.hpp"

MsgServ::MsgServ() {}

/**
 * @brief Returns the singleton instance of UserManager.
 *
 * This method ensures that only one instance of UserManager exists
 * throughout the program lifetime.
 *
 * @return Reference to the single ChanServ instance.
 */
MsgServ& MsgServ::getInstance()
{
	static MsgServ instance;
	return instance;
}

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
void MsgServ::recvMsg(int socket, int buffer_index, const std::string& nickname)
{
	char buffer[512];
	int bytes_received = recv(socket, buffer, sizeof(buffer) - 1, 0);
	if (bytes_received < 0)
		throw ReiceveException(socket, nickname);
	if (bytes_received == 0) // Connection closed by client
		throw ClientDisconnectedException(socket, nickname);
	buffer[bytes_received] = '\0';
	std::cout << "Received message: " << buffer << std::endl;
	if (completeMsg(buffer_index, buffer))
		throw std::runtime_error("Error: msg too long.");/** @todo throw MessageToLongException(); */
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
	sendReply(socket, cmd, "ircserv", clientTargetNickname);/** @todo ircServer ? server? other? */
}

void MsgServ::sendMessageReply(int socket, const MsgServ::Command& cmd)
{
	const std::string prefix = UserManager::getInstance().getUserPrefix(socket);
	sendReply(socket, cmd, prefix, "");
}

void MsgServ::sendReply(int socket, const MsgServ::Command& cmd, const std::string& prefix, const std::string& target)
{
	std::string reply = formatMsg(cmd, prefix, target);
	try
	{
		sendMsg(socket, reply);
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what();
		if (!target.empty())
			std::cerr << " to: " << target;
		std::cerr << std::endl;
	}
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
	ssize_t sent = send(socket, msg.c_str(), msg.size(), MSG_NOSIGNAL);
	if (sent == -1 && errno != EAGAIN)
		throw std::runtime_error("Error: cannot send message");
	if (sent > 0)
		msg.erase(0, sent);
}

void MsgServ::addNewBuffer()
{
	buffers_.push_back("");
}

/**
 * @brief Check if the this user has received a complete command ready to execute.
 *
 * @return true if the buffer contain at least one complete command, 0 otherwise.
 */
bool MsgServ::isCommandReady(int buffer_index) const
{
	std::string buffer = buffers_.at(buffer_index);
	return buffer.find("\r\n") != std::string::npos || buffer.find("\n") != std::string::npos;
}

/**
 * @brief Add the reiceved message to the user's buffer.
 * 
 * If the buffer was containing the beggin of a command, if will complete the command.
 * If the buffer was empty, it will simply sto the received message in the buffer.
 *
 * @return 1 if the total length of the current command is longer that 512, 0 otherwize.
 * 
 * @note You must the return value of this funciton and throw MessageToLongException() if true.
 */
int MsgServ::completeMsg(int buffer_index, const std::string &msg)
{
	std::string& buffer = buffers_.at(buffer_index);
	buffer += msg;
	if (buffer.size() > 512)
	{
		buffer.clear();
		return 1;
	}
	return 0;
}

/**
 * @brief Extracts and returns the next complete command from the buffer.
 * 
 * The function removes the command from the internal buffer after extraction.
 * 
 * @return The next command if available, or an empty string if no complete command is found.
 * 
 * @note According to the IRC protocol, "\r\n" is the standard line ending.
 *       If only "\n" is found (non-standard), it is still accepted for flexibility (netcat).
 */
std::string MsgServ::getCommand(int buffer_index)
{
	std::string& buffer = buffers_.at(buffer_index);
	size_t pos = buffer.find("\r\n");
	if (pos == std::string::npos)
	{
		pos = buffer.find("\n");
		if (pos == std::string::npos)
			return "";
		std::string command = buffer.substr(0, pos);
		buffer = buffer.substr(pos + 1);
		return command;
	}
	std::string command = buffer.substr(0, pos);
	buffer = buffer.substr(pos + 2);
	return command;
}

void MsgServ::removeBuffer(int buffer_index)
{
	if (buffer_index <= 0 || buffer_index >= static_cast<int>(buffers_.size()))
		return; // or throw std::out_of_range("Invalid index in deleteUser");
	buffers_.erase(buffers_.begin() + buffer_index);
}
