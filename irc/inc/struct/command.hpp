#pragma once

#include <iostream>
#include <vector>

struct Command {
	std::string name;
	std::vector<std::string> arg;
	bool hasTrailing;
};