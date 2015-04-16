#pragma once

#include <VrLib/VrLib.h>

#include <string>
#include <glm/glm.hpp>

namespace vrlib
{
	class User
	{
		std::string name;
	public:
		glm::mat4 matrix;


		User(std::string name);
		~User();

		std::string getName();
		glm::vec3 getEyePosition(int eye);

	};

}