#include "MsgServ.hpp"

#include <cerrno>
#include <iomanip>
#include <sstream>

#include "Server.hpp"
#include "ClientDisconnectedException.hpp"
#include "ReiceveException.hpp"
#include "UserManager.hpp"
#include "command.hpp"

MsgServ::MsgServ(): Service("MsgServ") {}

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
void MsgServ::recvMsg(int socket, int buffer_index)
{
	char buffer[BUFFER_SIZE];
	int bytes_received = recv(socket, buffer, sizeof(buffer) - 1, 0);
	if (bytes_received < 0)
		throw ReiceveException();
	if (bytes_received == 0) // Connection closed by client
		throw ClientDisconnectedException();
	buffer[bytes_received] = '\0';
	std::cout << "Received message: " << buffer << std::endl;
	if (completeMsg(buffer_index, buffer))
		throw std::runtime_error("Error: msg too long.");/** @todo throw MessageToLongException(); clear buffer ? */
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

void MsgServ::receiveCommand(const int senderSocket, const std::string& cmd)
{
	(void) senderSocket;
	(void) cmd;
}

void MsgServ::sendNumericReply(const int receiverSocket, const int code) const
{
	Message msg;
	msg.hasTrailing = false;
	sendNumericReply(receiverSocket, code, msg);
}

void MsgServ::sendNumericReply(const int receiverSocket, const int code, const std::string& trailing) const
{
	Message msg;
	msg.hasTrailing = true;
	msg.trailing = trailing;
	sendNumericReply(receiverSocket, code, msg);
}

void MsgServ::sendNumericReply(const int receiverSocket, const int code, const std::vector<std::string> &arg) const
{
	Message msg;
	msg.arg = arg;
	msg.hasTrailing = false;
	sendNumericReply(receiverSocket, code, msg);
}

void MsgServ::sendNumericReply(const int receiverSocket, const int code, const std::vector<std::string> &arg, const std::string &trailing) const
{
	Message msg;
	msg.arg = arg;
	msg.hasTrailing = true;
	msg.trailing = trailing;
	sendNumericReply(receiverSocket, code, msg);
}

void MsgServ::sendNumericReply(const int receiverSocket, const int code, Message& msg) const
{
	const std::string SERVER_PREFIX = "ircserv";

	const std::string nickname = UserManager::getInstance().findUserNickname(receiverSocket);
	msg.arg.insert(msg.arg.begin(), nickname);
	msg.receiverSocket = receiverSocket;
	msg.senderPrefix = SERVER_PREFIX;
	msg.commandId = convertCodeToCommandId(code);
	sendReply(msg);
}

void MsgServ::sendMessageReply(const int senderSocket, const int receiverSocket, const Command& cmd) const
{
	Message msg;
	msg.arg = cmd.arg;
	msg.hasTrailing = cmd.hasTrailing;
	if (msg.hasTrailing)
	{
		msg.arg.pop_back();
		msg.trailing = cmd.arg.back();
	}
	sendMessageReply(senderSocket, receiverSocket, cmd.name, msg);
}

void MsgServ::sendMessageReply(const int senderSocket, const int receiverSocket, const std::string& commandId) const
{
	Message msg;
	msg.hasTrailing = false;
	sendMessageReply(senderSocket, receiverSocket, commandId, msg);
}

void MsgServ::sendMessageReply(const int senderSocket, const int receiverSocket, const std::string& commandId, const std::string &trailing) const
{
	Message msg;
	msg.hasTrailing = true;
	msg.trailing = trailing;
	sendMessageReply(senderSocket, receiverSocket, commandId, msg);
}

void MsgServ::sendMessageReply(const int senderSocket, const int receiverSocket, const std::string& commandId, const std::vector<std::string>& arg) const
{
	Message msg;
	msg.arg = arg;
	msg.hasTrailing = false;
	sendMessageReply(senderSocket, receiverSocket, commandId, msg);
}

void MsgServ::sendMessageReply(const int senderSocket, const int receiverSocket, const std::string& commandId, const std::vector<std::string>& arg,
	const std::string& trailing) const
{
	Message msg;
	msg.arg = arg;
	msg.hasTrailing = true;
	msg.trailing = trailing;
	sendMessageReply(senderSocket, receiverSocket, commandId, msg);
}

void MsgServ::sendMessageReply(const int senderSocket, const int receiverSocket, const std::string &commandId, Message& msg) const
{
	msg.senderPrefix = UserManager::getInstance().findUserPrefix(senderSocket);
	msg.receiverSocket = receiverSocket;
	msg.commandId = commandId;
	sendReply(msg);
}

void MsgServ::sendNoticeReply(const std::string serviceName, const int receiverSocket, const std::string &trailing)
{
	std::string nickname = UserManager::getInstance().findUserNickname(receiverSocket);
	Message msg;
	msg.receiverSocket = receiverSocket;
	msg.senderPrefix = serviceName;
	msg.commandId = "NOTICE";
	msg.arg.push_back(nickname);
	msg.hasTrailing = true;
	msg.trailing = trailing;
	sendReply(msg);
}

void MsgServ::sendReply(const Message &msg) const
{
	std::string reply = formatMsg(msg);
	try
	{
		sendMsg(msg.receiverSocket, reply);
	}
	catch (const std::exception& e)
	{
		if (!Server::getInstance().isUp()) /** @todo skip for tester */
			return;
		std::cerr << e.what();
		//if (!target.empty())
			//std::cerr << " to: " << target;
		std::cerr << std::endl;
	}
}

std::string MsgServ::formatMsg(const Message &msg) const
{
	std::string reply = ":" + msg.senderPrefix + " " + msg.commandId;
	for (std::size_t i = 0; i < msg.arg.size(); ++i)
	{
		if (!msg.arg.at(i).empty())
			reply += " " + msg.arg.at(i);
	}
	if (msg.hasTrailing)
		reply += " :" + msg.trailing; 
	return reply + "\r\n";
}

/**
 * @brief Sends a message over the given socket.
 *
 * @param receiverSocket The socket file descriptor to send the message through.
 * @param msg The message to send; the sent part will be erased from this string.
 *
 * @throws SendException if sending fails for reasons other than EAGAIN.
 *
 * @note Currently uses a temporary nickname placeholder for error reporting.
 */
void MsgServ::sendMsg(int receiverSocket, std::string& msg) const
{
	/** std::string nickname = "tmpNickname"; @todo find right nickname or simply get a reference to user so that we can access nickname and socket ??*/
	if (msg.empty())
		return;
	ssize_t sent = send(receiverSocket, msg.c_str(), msg.size(), MSG_NOSIGNAL);
	if (sent == -1 && errno != EAGAIN)
		throw std::runtime_error("Error: cannot send message"); /** @todo it s a right exeption ? */
	if (sent > 0)
		msg.erase(0, sent);
}

const std::string MsgServ::convertCodeToCommandId(const int code) const
{
	std::stringstream codeName;
	codeName << std::setw(3) << std::setfill('0') << code;
	return codeName.str();
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
	if (buffer.size() > BUFFER_SIZE)
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
	std::size_t posOffset = 2;
	std::size_t pos = buffer.find("\r\n");
	if (pos == std::string::npos)
	{
		pos = buffer.find("\n");
		if (pos == std::string::npos)
			return "";
		posOffset = 1;
	}
	const std::string command = buffer.substr(0, pos);
	buffer = buffer.substr(pos + posOffset);
	return command;
}

void MsgServ::removeBuffer(int buffer_index)
{
	if (buffer_index <= 0 || buffer_index >= static_cast<int>(buffers_.size()))
		return; // or throw std::out_of_range("Invalid index in deleteUser");
	buffers_.erase(buffers_.begin() + buffer_index);
}
