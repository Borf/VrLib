#pragma once

#include <glm/glm.hpp>

namespace vrlib
{
	namespace math
	{
		class Frustum
		{
		public:
			glm::vec4 planes[6];
			glm::mat4 projectionMatrix;
			glm::mat4 modelviewMatrix;


			Frustum() {};
			Frustum(const glm::mat4 &projectionMatrix, const glm::mat4 &modelviewMatrix);
			void setFromMatrix(const glm::mat4 &projectionMatrix, const glm::mat4 &modelviewMatrix);

			glm::vec3 getCenter();

			bool sphereInFrustum(const glm::vec3 &position, float radius) const;

		};

	}
}