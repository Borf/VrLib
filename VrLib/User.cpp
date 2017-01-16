#define GLM_SWIZZLE
#include "User.h"
#include <glm/glm.hpp>

namespace vrlib
{
	User::User(std::string name)
	{
		this->name = name;
		matrix = glm::mat4();
	}

	User::~User()
	{
	}

	float eyeDist = 0.025f; //cave
	//float eyeDist = 0.1f; //oculus

	glm::vec3 User::getEyePosition(int eye)
	{
		if (eye == 0)
			return glm::vec3(matrix * glm::vec4(0, 0, 0, 1));
		if (eye == 1)
			return glm::vec3(matrix * glm::vec4(-eyeDist, 0, 0, 1));
		//if(eye == 2)
		return glm::vec3(matrix * glm::vec4(eyeDist, 0, 0, 1));
	}

	std::string User::getName()
	{
		return name;
	}
}