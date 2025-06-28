#include "User.hpp"

User::User(struct sockaddr_in clientAddr): nickname_("*"), isConnected_(false), clientAddr_(clientAddr) {}

/**
 * @brief Checks if the user's nickname matches the given nickname.
 *
 * @param nickname The nickname to compare with the user's nickname.
 *
 * @return true if the nicknames are equal, false otherwise.
 */
bool User::is(std::string nickname)
{
	return (nickname_ == nickname);
}

/**
 * @brief Set the user's nickname.
 */
void User::setNickname(const std::string& nickname)
{
	nickname_ = nickname;
}

/**
 * @brief Set the user's username.
 */
void User::setUsername(const std::string& username)
{
	username_ = username;
}

/**
 * @brief Set the user's realname.
 */
void User::setRealName(const std::string& realname)
{
	realName_ = realname;
}

/**
 * @brief Returns the user's nickname.
 *
 * @return Constant reference to the nickname string.
 */
const std::string& User::getNickname() const
{
	return nickname_;
}

/**
 * @brief Returns the user's username.
 *
 * @return Constant reference to the username string.
 */
const std::string& User::getUsername() const
{
	return username_;
}

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
struct sockaddr_in User::getAddress()
{
	return (clientAddr_);
}


bool User::isCommandReady() const
{
	std::cout << "Actual buffer_: " << buffer_ << " -> size: " << buffer_.size() << std::endl;
	
	return buffer_.find("\r\n") != std::string::npos || buffer_.find("\n") != std::string::npos;
}

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

std::string User::getCommand()
{
	size_t pos = buffer_.find("\r\n");
	if (pos == std::string::npos)
	{
		pos = buffer_.find("\n");
		if (pos == std::string::npos)
			return ""; // Should not arrive maybe trhow an exception
		std::string command = buffer_.substr(0, pos);
		buffer_ = buffer_.substr(pos + 1);
		return command;
	}
	std::string command = buffer_.substr(0, pos);
	buffer_ = buffer_.substr(pos + 2);
	return command;
}

void User::validConnection()
{
	isConnected_ = true;
}

bool User::isConnected() const
{
	return (isConnected_);
}

bool User::isNickRegister() const
{
	return (nickname_ != "*");
}

bool User::isUsernameRegister() const
{
	return (!username_.empty());
}