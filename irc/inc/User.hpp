#pragma once

#include <iostream>

#include "status.hpp"

class UserTester;/** @todo remove before push */

class User
{
public:
	User();

	void passToNextConnectionStep();
	
	static const std::string getDefaultPrefix();
	static const std::string getDefaultNickname();
	static const std::string getDefaultUsername();
	const std::string getPrefix() const;
	const std::string& getNickname() const;
	const std::string& getUsername() const;
	const Status& getStatus() const;
	void setNickname(const std::string& nickname);
	void setUsername(const std::string& username);
	bool is(const std::string& nickname) const;

private:
	std::string nickname_;
	std::string username_;
	Status status_;

	friend class UserTester;/** @todo remove before push */
};