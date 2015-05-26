#include "Ray.h"

namespace vrlib
{
	namespace math
	{
		Ray::Ray(const glm::vec3 &origin, const glm::vec3 &dir)
		{
			this->mOrigin = origin;
			this->mDir = dir;

			invDir = glm::vec3(1.0f / dir.x, 1.0f / dir.y, 1.0f / dir.z);
			sign[0] = (invDir.x < 0);
			sign[1] = (invDir.y < 0);
			sign[2] = (invDir.z < 0);
		}

		vrlib::math::Ray operator*(const glm::mat4& matrix, const Ray& ray)
		{
			glm::vec4 transOrigin = matrix * glm::vec4(ray.mOrigin, 1);
			glm::vec4 transDir = matrix * glm::vec4(ray.mDir, 0);
			return Ray(glm::vec3(transOrigin), glm::vec3(transDir));
		}



	}
}
