#include "User.hpp"

User::User(): nickname_(getDefaultNickname()), username_(getDefaultUsername()), status_(DISCONNECTED) {}

/**
 * @brief Checks if the user's nickname matches the given nickname.
 *
 * @param nickname The nickname to compare with the user's nickname.
 *
 * @return true if the nicknames are equal, false otherwise.
 */
bool User::is(const std::string& nickname) const { return (nickname_ == nickname); }

Status User::getStatus() const { return status_; }

/**
 * @brief Marks the user as connected.
 * 
 * @note The user must give the good server password with the PASS command before excuting this function.
 */
void User::passToNextConnectionStep()
{
	/** @todo do exception to not overflow ? */
	status_ = (Status)(status_ + 1);
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

const std::string User::getDefaultUsername() const { return ""; }

/**
 * @brief Returns the user's username.
 *
 * @return Constant reference to the username string.
 */
const std::string& User::getUsername() const { return username_; }
