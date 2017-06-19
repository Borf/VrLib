#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>

namespace vrlib
{
	namespace util
	{
		std::string replace(std::string str, const std::string &toReplace, const std::string &replacement);
		std::vector<std::string> split(std::string toSplit, const std::string &seperator);

		std::vector<std::string> scandir(const std::string &path, bool recursive = false);

		glm::vec3 randomHsv();
		glm::vec3 hsv2rgb(glm::vec3 hsv);
		glm::vec3 rgb2hsv(glm::vec3 rgb);

		glm::vec4 hex2rgb(const std::string &hex);
		std::string rgb2hex(const glm::vec4 &rgb);

		float random(float min, float max);

		std::string getGuid();

	}
}