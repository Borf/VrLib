#include "aabb.h"
#include <glm/glm.hpp>
#include "Ray.h"
#include <math.h>
#include <GL/glew.h>

namespace vrlib
{
	namespace math
	{

		glm::vec3 AABB::center() const
		{
			return (bounds[0] + bounds[1]) / 2.0f;
		}

		/*
		Check if a ray has collision with this boundingbox. Thanks to http://www.cs.utah.edu/~awilliam/box/box.pdf
		Author: Bas Rops - 09-06-2014
		Last edit: Bas Rops - dd-mm-yyyy
		*/
		bool AABB::hasRayCollision(const Ray& r, float minDistance, float maxDistance)
		{
			float tmin, tmax, tymin, tymax, tzmin, tzmax;

			tmin = (bounds[r.sign[0]].x - r.mOrigin.x) * r.invDir.x;
			tmax = (bounds[1 - r.sign[0]].x - r.mOrigin.x) * r.invDir.x;

			tymin = (bounds[r.sign[1]].y - r.mOrigin.y) * r.invDir.y;
			tymax = (bounds[1 - r.sign[1]].y - r.mOrigin.y) * r.invDir.y;

			if ((tmin > tymax) || (tymin > tmax))
				return false;

			if (tymin > tmin)
				tmin = tymin;

			if (tymax < tmax)
				tmax = tymax;

			tzmin = (bounds[r.sign[2]].z - r.mOrigin.z) * r.invDir.z;
			tzmax = (bounds[1 - r.sign[2]].z - r.mOrigin.z) * r.invDir.z;

			if ((tmin > tzmax) || (tzmin > tmax))
				return false;

			if (tzmin > tmin)
				tmin = tzmin;

			if (tzmax < tmax)
				tmax = tzmax;

			return ((tmin < maxDistance) && (tmax > minDistance));
		}

		/*
Check if a ray has collision with this boundingbox. Thanks to http://www.cs.utah.edu/~awilliam/box/box.pdf
Author: Bas Rops - 09-06-2014
Last edit: Bas Rops - dd-mm-yyyy
*/
		float AABB::getRayCollision(const Ray& r, float minDistance, float maxDistance)
		{
			float tmin, tmax, tymin, tymax, tzmin, tzmax;

			tmin = (bounds[r.sign[0]].x - r.mOrigin.x) * r.invDir.x;
			tmax = (bounds[1 - r.sign[0]].x - r.mOrigin.x) * r.invDir.x;

			tymin = (bounds[r.sign[1]].y - r.mOrigin.y) * r.invDir.y;
			tymax = (bounds[1 - r.sign[1]].y - r.mOrigin.y) * r.invDir.y;

			if ((tmin > tymax) || (tymin > tmax))
				return -1;

			if (tymin > tmin)
				tmin = tymin;

			if (tymax < tmax)
				tmax = tymax;

			tzmin = (bounds[r.sign[2]].z - r.mOrigin.z) * r.invDir.z;
			tzmax = (bounds[1 - r.sign[2]].z - r.mOrigin.z) * r.invDir.z;

			if ((tmin > tzmax) || (tzmin > tmax))
				return -1;

			if (tzmin > tmin)
				tmin = tzmin;

			if (tzmax < tmax)
				tmax = tzmax;

			if ((tmin < maxDistance) && (tmax > minDistance))
				return tmin;

			return tmin;
		}

		void AABB::add(const AABB & other, const glm::mat4 &transform)
		{
			glm::vec3 obounds1(transform * glm::vec4(other.bounds[0], 1));
			glm::vec3 obounds2(transform * glm::vec4(other.bounds[1], 1));

			for (int i = 0; i < 3; i++)
			{
				bounds[0][i] = glm::min(bounds[0][i], obounds1[i]);
				bounds[1][i] = glm::max(bounds[1][i], obounds2[i]);
			}
		}
	}
}