#pragma once

#include "Client.hpp"

#include <iostream>

#include "status.hpp"

class UserTester;/** @todo remove before push */

class User: public Client
{
public:
	User();
	// ~User();

	void passToNextConnectionStep();
	
	static const std::string getDefaultPrefix();
	static const std::string getDefaultNickname();
	static const std::string getDefaultUsername();
	const std::string getPrefix() const;
	const std::string& getNickname() const;
	const std::string& getUsername() const;
	const Status& getStatus() const;
	void setNickname(const std::string& nickname);
	void setUsername(const std::string& username); /** @todo ignore ?? */
	void setRealName(const std::string& realname);
	bool is(const std::string& nickname) const;

private:
	std::string nickname_;
	std::string username_;
	std::string realName_; /** @todo ignore ?? */
	Status status_;

	friend class UserTester;/** @todo remove before push */
};