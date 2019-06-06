#pragma once

#include <glm/glm.hpp>


namespace vrlib
{
	namespace math
	{

	class Ray;

	class AABB
	{
	public:
		glm::vec3 bounds[2];

		AABB() { }
		AABB(glm::vec3 _min, glm::vec3 _max)
		{
			bounds[0] = _min;
			bounds[1] = _max;
		}

		glm::vec3 center() const;

		bool hasRayCollision(const Ray& r, float minDistance, float maxDistance);
		float getRayCollision(const Ray& r, float minDistance, float maxDistance);

		void add(const AABB& other, const glm::mat4 &transform);

		inline glm::vec3 getSize() { return bounds[1] - bounds[0]; }
	};
	}
}
