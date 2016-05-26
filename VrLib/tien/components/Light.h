#pragma once

#include <glm/glm.hpp>

namespace vrlib
{
	namespace tien
	{
		namespace components
		{
			class Light
			{
			public:
				enum class Type
				{
					point,
					directional,
					spot
				} type;
				float intensity;
				glm::vec4 color;

			};
		}
	}
}