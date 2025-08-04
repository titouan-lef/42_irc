#pragma once

#include "Client.hpp"

#include <iostream>

#include "status.hpp"

class UserTester;

class User: public Client
{
public:
	User();
	// ~User();

	bool is(const std::string& nickname) const;
	Status getStatus() const;
	void passToNextConnectionStep();
	
	static const std::string getDefaultNickname();
	static const std::string getDefaultPrefix();
	const std::string& getNickname() const;
	const std::string& getUsername() const;
	void setNickname(const std::string& nickname);
	void setUsername(const std::string& username); /** @todo ignore ?? */
	void setRealName(const std::string& realname);
	const std::string getPrefix() const;

private:
	const std::string getDefaultUsername() const;

	std::string nickname_;
	std::string username_;
	std::string realName_; /** @todo ignore ?? */
	Status status_;

	friend class UserTester;
};