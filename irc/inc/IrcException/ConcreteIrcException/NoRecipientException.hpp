#pragma once

#include "IrcException.hpp"

#include <iostream>

class NoRecipientException: public IrcException
{
public:
	virtual ~NoRecipientException() throw() {}

	NoRecipientException(const std::string& commandName);
};