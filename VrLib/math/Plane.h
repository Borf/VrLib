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
			glm::vec3 normal;
			float f;
		public:
			Plane(glm::vec3 a, glm::vec3 b, glm::vec3 c);
			Plane(glm::vec3 normal, float f);

			glm::vec3 getNormal() const;
			float		getOffset() const;

			glm::vec3 getCollisionPoint(const Ray &) const;

		};
	}
}