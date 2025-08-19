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

MsgServ& MsgServ::getInstance()
{
	static MsgServ instance;
	return instance;
}

/**
 * @brief Receives a message from the specified socket and stock it in its buffer.
 * @param receiverSocket The socket file descriptor to receive data from.
 * @param buffer_index Index of the buffer in buffer list.
 * @throws ReiceveException, ClientDisconnectedException.
 */
void MsgServ::recvMsg(int receiverSocket, int buffer_index)
{
	char buffer[BUFFER_SIZE];
	int bytes_received = recv(receiverSocket, buffer, sizeof(buffer) - 1, 0);
	if (bytes_received < 0)
		throw ReiceveException();
	if (bytes_received == 0) // Connection closed by client
		throw ClientDisconnectedException();
	buffer[bytes_received] = '\0';
	std::cout << "Received message: " << buffer << std::endl;
	if (completeMsg(buffer_index, buffer))
		throw std::runtime_error("Error: msg too long.");/** @todo throw MessageToLongException();*/
}

void MsgServ::receiveCommand(const int senderSocket, const std::string& cmd)
{
	(void) senderSocket;
	(void) cmd;
}

void MsgServ::sendNumericReply(const int receiverSocket, const int code, const std::string& trailing) const
	{ sendNumericReply(receiverSocket, code, NULL, &trailing); }

void MsgServ::sendNumericReply(const int receiverSocket, const int code, const std::vector<std::string> &arg) const
	{ sendNumericReply(receiverSocket, code, &arg); }

void MsgServ::sendNumericReply(const int receiverSocket, const int code, const std::vector<std::string> &arg, const std::string &trailing) const
	{ sendNumericReply(receiverSocket, code, &arg, &trailing); }

void MsgServ::sendNumericReply(const int receiverSocket, const int code, const std::vector<std::string>* arg, const std::string* trailing) const
{
	const std::string SERVER_PREFIX = "ircserv";

	Message msg = initMessage(receiverSocket, SERVER_PREFIX, convertCodeToCommandId(code), trailing);
	const std::string nickname = UserManager::getInstance().findUserNickname(receiverSocket);
	msg.arg.push_back(nickname);
	if (arg != NULL)
		msg.arg.insert(msg.arg.end(), arg->begin(), arg->end());
	sendReply(msg);
}

void MsgServ::sendMessageReply(const int senderSocket, const int receiverSocket, const Command& cmd) const
{
	if (cmd.hasTrailing)
	{
		std::vector<std::string> arg = cmd.arg;
		arg.pop_back();
		sendMessageReply(senderSocket, receiverSocket, cmd.name, &arg, &cmd.arg.back());
	}
	else
		sendMessageReply(senderSocket, receiverSocket, cmd.name, &cmd.arg);
}

void MsgServ::sendMessageReply(const int senderSocket, const int receiverSocket, const std::string& commandId, const std::string &trailing) const
	{ sendMessageReply(senderSocket, receiverSocket, commandId, NULL, &trailing); }

void MsgServ::sendMessageReply(const int senderSocket, const int receiverSocket, const std::string& commandId, const std::vector<std::string>& arg) const
	{ sendMessageReply(senderSocket, receiverSocket, commandId, &arg); }

void MsgServ::sendMessageReply(const int senderSocket, const int receiverSocket, const std::string& commandId, const std::vector<std::string>& arg, const std::string& trailing) const
	{ sendMessageReply(senderSocket, receiverSocket, commandId, &arg, &trailing); }

void MsgServ::sendMessageReply(const int senderSocket, const int receiverSocket, const std::string &commandId, const std::vector<std::string>* arg, const std::string* trailing) const
{
	const std::string senderPrefix = UserManager::getInstance().findUserPrefix(senderSocket);
	Message msg = initMessage(receiverSocket, senderPrefix, commandId, trailing);
	if (arg != NULL)
		msg.arg = *arg;
	sendReply(msg);
}

MsgServ::Message MsgServ::initMessage(const int receiverSocket, const std::string &senderPrefix, const std::string &commandId, const std::string *trailing) const
{
	Message msg;
	msg.receiverSocket = receiverSocket;
	msg.senderPrefix = senderPrefix;
	msg.commandId = commandId;
	if (trailing == NULL)
		msg.hasTrailing = false;
	else
	{
		msg.hasTrailing = true;
		msg.trailing = *trailing;
	}
	return msg;
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
		std::cerr << e.what() << std::endl;
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
 */
void MsgServ::sendMsg(int receiverSocket, std::string& msg) const
{
	if (msg.empty())
		return;
	ssize_t sent = send(receiverSocket, msg.c_str(), msg.size(), MSG_NOSIGNAL); // MSG_NOSIGNAL -> no SIGPIPE is connection is closed
	if (sent == 0)
		return;
	if (sent == -1 && errno != EAGAIN) /** @todo check EAGAIN and commant it */
		throw std::runtime_error("Error: cannot send message");
	if (sent > 0)
		msg.erase(0, sent); // remove only the data from the mesaage how have been sent
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
 * @return true if the buffer contain at least one complete command, 0 otherwise.
 */
bool MsgServ::isCommandReady(int buffer_index) const
{
	const std::string& buffer = buffers_.at(buffer_index);
	return buffer.find("\r\n") != std::string::npos || buffer.find("\n") != std::string::npos;
}

/**
 * @brief Add the reiceved message to the user's buffer.
 * @return 1 if the total length of the current command is longer that 512, 0 otherwize.
 * @note You must the return value of this funciton and throw MessageToLongException() if true.
 */
bool MsgServ::completeMsg(int buffer_index, const std::string &msg)
{
	std::string& buffer = buffers_.at(buffer_index);
	buffer += msg;
	if (buffer.size() > BUFFER_SIZE)
	{
		buffer.clear();
		return true;
	}
	return false;
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
		return;
	buffers_.erase(buffers_.begin() + buffer_index);
}
