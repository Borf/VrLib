#pragma once

#include "../Component.h"
#include <glm/glm.hpp>

namespace vrlib
{
	namespace tien
	{
		namespace components
		{
			class SkyBox : public Component 
			{
			public:
				bool initialized = false;
				virtual void initialize() = 0;
				virtual void render(const glm::mat4 &projectionMatrix, const glm::mat4 &modelviewMatrix) = 0;
			};
		}
	}
}