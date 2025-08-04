#pragma once

#include <iostream>
#include <vector>

#include "status.hpp"

struct Command;

class ACommand
{
public:
	virtual ~ACommand(){}

	virtual void execute(const Command& cmd, const int senderSocket, const Status& userStatus) const = 0;

protected:
	ACommand(const Status& statusMin, const Status& statusMax, const std::size_t nbArgMin, const std::size_t nbArgMax);

	void checkUserStatus(const Status& userStatus) const;
	void checkArgSize(const Command& cmd) const;
	std::vector<std::string> splitTargets(const std::string& str) const;

	const Status statusMin_;
	const Status statusMax_;
	const std::size_t nbArgMin_;
	const std::size_t nbArgMax_;

private:
	void checkUserRequiredStatus(const Status& userStatus) const;
	void checkUserExceededStatus(const Status& userStatus) const;
};