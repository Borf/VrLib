#pragma once

#include <glm/glm.hpp>

namespace vrlib
{
	namespace math
	{
		class Frustum
		{
			glm::vec4 planes[6];
		public:
			Frustum(const glm::mat4 &projectionMatrix, const glm::mat4 &modelviewMatrix);
		};

	}
}