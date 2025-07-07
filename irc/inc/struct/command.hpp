#pragma once

#include <iostream>
#include <vector>

struct Command {
	std::string name;
	std::string trailing;
	std::vector<std::string> arg;
};