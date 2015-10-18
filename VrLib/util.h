#pragma once

#include <string>
#include <vector>

namespace vrlib
{
	namespace util
	{

		std::vector<std::string> split(std::string toSplit, const std::string &seperator);

		std::vector<std::string> scandir(const std::string &path);

	}
}