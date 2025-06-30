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
 * @brief Marks the user as connected.
 * 
 * @note The user must give the good server password with the PASS command before excuting this function.
 */
void User::validConnection()
{
	isConnected_ = true;
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

const std::string User::getDefaultUsername() const { return ""; }

/**
 * @brief Returns the user's username.
 *
 * @return Constant reference to the username string.
 */
const std::string& User::getUsername() const { return username_; }
