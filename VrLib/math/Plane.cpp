#include "Plane.h"
#include "Ray.h"

#include <glm/glm.hpp>

namespace vrlib
{
	namespace math
	{

		Plane::Plane(glm::vec3 a, glm::vec3 b, glm::vec3 c)
		{
			normal = glm::normalize(glm::cross(b - a, c - a));
		}

		Plane::Plane(glm::vec3 normal, float f)
		{
			this->normal = normal;
			this->f = f;
		}

		glm::vec3 Plane::getCollisionPoint(const Ray& ray) const
		{
			float Denominator = glm::dot(ray.mDir, this->normal);
			if (fabs(Denominator) <= 0.001f) // Parallel to the plane
			{
				return f*normal;
			}
			float Numerator = glm::dot(ray.mOrigin, this->normal) + this->f;
			float t = -Numerator / Denominator;
			return ray.mOrigin + t * ray.mDir;
		}
	}
}