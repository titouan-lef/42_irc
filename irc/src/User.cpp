#include "User.hpp"

User::User(): nickname_(getDefaultNickname()), username_(getDefaultUsername()), status_(DISCONNECTED) {}

void User::passToNextConnectionStep()
{
	if (status_ == CONNECTED)
		return;
	status_ = (Status)(status_ + 1);
}

const std::string User::getDefaultPrefix() { return "*!*@localhost"; }

const std::string User::getDefaultNickname() { return "*"; }

const std::string User::getDefaultUsername() { return ""; }

const std::string User::getPrefix() const
{
	std::string prefix;

	prefix.append(nickname_);
	prefix.append("!");
	prefix.append(username_);
	prefix.append("@localhost");
	return prefix;
}

const std::string& User::getNickname() const { return nickname_; }

const std::string& User::getUsername() const { return username_; }

const Status& User::getStatus() const { return status_; }

void User::setNickname(const std::string& nickname) { nickname_ = nickname; }

void User::setUsername(const std::string& username) { username_ = username; }

bool User::is(const std::string& nickname) const { return (nickname_ == nickname); }
