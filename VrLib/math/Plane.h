#pragma once

#include <glm/glm.hpp>

#pragma warning (disable: 4251)

namespace vrlib
{
	namespace math
	{
		class Ray;

		class Plane
		{
		public:
			glm::vec3 normal;
			float f;

			Plane() {};
			Plane(glm::vec3 a, glm::vec3 b, glm::vec3 c);
			Plane(glm::vec3 normal, float f);

			glm::vec3 getCollisionPoint(const Ray &) const;

		};
	}
}