#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>

namespace vrlib
{
	namespace util
	{

		std::vector<std::string> split(std::string toSplit, const std::string &seperator);

		std::vector<std::string> scandir(const std::string &path);

		glm::vec3 randomHsv();

		float random(float min, float max);

		std::string getGuid();

	}
}