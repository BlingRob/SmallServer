#pragma once

#include <memory>
#include <string>

class Parser
{

public:

	std::string Get(const std::string& request, const std::string& dir);
};
