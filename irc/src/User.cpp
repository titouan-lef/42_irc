#include "User.hpp"

User::User(struct sockaddr_in clientAddr):
	nickname_(getDefaultNickname()), username_(getDefaultUsername()), isConnected_(false), clientAddr_(clientAddr) {}

/**
 * @brief Checks if the user's nickname matches the given nickname.
 *
 * @param nickname The nickname to compare with the user's nickname.
 *
 * @return true if the nicknames are equal, false otherwise.
 */
bool User::is(const std::string& nickname) const
{
	return (nickname_ == nickname);
}

/**
 * @brief Checks if the user is connected.
 *
 * A user is consider connected if he succesfully gave the write server password.
 * 
 * @return true if the user is connected, false otherwise.
 */
bool User::isConnected() const
{
	return isConnected_;
}

/**
 * @brief Checks if the user has registered a nickname.
 *
 * @return true if the nickname is different from the default "*", false otherwise.
 */
bool User::isNickRegister() const
{
	return (nickname_ != getDefaultNickname());
}

/**
 * @brief Checks if the user has registered a username.
 *
 * @return true if the username string is not empty, false otherwise.
 */
bool User::isUsernameRegister() const
{
	return (username_ != getDefaultUsername());
}

/**
 * @brief Check if the this user has received a complete command ready to execute.
 *
 * @return true if the buffer contain at least one complete command, 0 otherwise.
 */
bool User::isCommandReady() const
{
	std::cout << "Actual buffer_: " << buffer_ << " -> size: " << buffer_.size() << std::endl;
	
	return buffer_.find("\r\n") != std::string::npos || buffer_.find("\n") != std::string::npos;
}

/**
 * @brief Marks the user as connected.
 * 
 * @note The user must give the good server password with the PASS command before excuting this function.
 */
void User::validConnection()
{
	isConnected_ = true;
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
int User::completeMsg(const std::string &msg)
{
	buffer_ += msg;
	if (buffer_.size() > 512)
	{
		buffer_.clear();
		return 1;
	}
	return 0;
}

const std::string User::getDefaultNickname() { return "*"; }

const std::string User::getDefaultPrefix() { return "*!*@localhost"; }

/**
 * @brief Returns the user's nickname.
 *
 * @return Constant reference to the nickname string.
 */
const std::string& User::getNickname() const { return nickname_; }

/**
 * @brief Set the user's nickname.
 */
void User::setNickname(const std::string& nickname) { nickname_ = nickname; }

/**
 * @brief Set the user's username.
 */
void User::setUsername(const std::string& username) { username_ = username; }

/**
 * @brief Set the user's realname.
 */
void User::setRealName(const std::string& realname) { realName_ = realname; }

/**
 * @brief Constructs and returns the IRC prefix for the user.
 *
 * The prefix has the format: ":nickname!username@localhost"
 * 
 * @return The constructed prefix string.
 *
 * @note The hostname is currently hardcoded as "localhost".
 */
const std::string User::getPrefix() const
{
	std::string prefix;

	prefix.append(nickname_);
	prefix.append("!");
	prefix.append(username_);
	prefix.append("@localhost");
	return prefix;
}

/**
 * @brief Returns the socket address information of the user.
 *
 * @return A copy of the user's sockaddr_in structure.
 */
struct sockaddr_in User::getAddress() const { return clientAddr_; }

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
std::string User::getCommand()
{
	size_t pos = buffer_.find("\r\n");
	if (pos == std::string::npos)
	{
		pos = buffer_.find("\n");
		if (pos == std::string::npos)
			return "";
		std::string command = buffer_.substr(0, pos);
		buffer_ = buffer_.substr(pos + 1);
		return command;
	}
	std::string command = buffer_.substr(0, pos);
	buffer_ = buffer_.substr(pos + 2);
	return command;
}

const std::string User::getDefaultUsername() const { return ""; }

/**
 * @brief Returns the user's username.
 *
 * @return Constant reference to the username string.
 */
const std::string& User::getUsername() const { return username_; }
