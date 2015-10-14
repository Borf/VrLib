#pragma once


#include <glm/glm.hpp>

namespace vrlib
{
	namespace math
	{
		class Plane;

		class Ray
		{
		public:
			glm::vec3 mOrigin;
			glm::vec3 mDir;
			glm::vec3 invDir;
			int sign[3];

			Ray() {}
			Ray(const glm::vec3 &origin, const glm::vec3 &dir);

			bool planeIntersection(const Plane &plane, float &t) const;
			bool LineIntersectPolygon(const glm::vec3* vertices, int count, float &t) const;
		};

		Ray operator * (const glm::mat4& matrix, const Ray& ray);

	}
}