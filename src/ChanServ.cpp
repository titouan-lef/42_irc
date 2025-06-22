#include "ChanServ.hpp"

ChanServ::ChanServ() {}


ChanServ& ChanServ::getInstance()
{
	static ChanServ instance;
	return instance;
}

Channel& ChanServ::findChannelFor(std::string name, User& user)
{
	for (std::vector<Channel>::iterator it = channels_.begin(); it < channels_.end(); ++it)
	{
		if (it->getName() == name)
			return *it;
	}
	return builtNewChannel(name, user);
}

Channel& ChanServ::builtNewChannel(std::string name, User& user)
{
	Channel newChannel(name, user);
	channels_.push_back(newChannel);
	return channels_.back();
}
