#pragma once


#include <glm/glm.hpp>

namespace vrlib
{
	namespace math
	{

		class Ray
		{
		public:
			glm::vec3 mOrigin;
			glm::vec3 mDir;
			glm::vec3 invDir;
			int sign[3];

			Ray() {}
			Ray(glm::vec3 origin, glm::vec3 dir)
			{
				this->mOrigin = origin;
				this->mDir = dir;
		
				invDir = glm::vec3(1.0f / dir.x, 1.0f / dir.y, 1.0f / dir.z);
				sign[0] = (invDir.x < 0);
				sign[1] = (invDir.y < 0);
				sign[2] = (invDir.z < 0);
			}
		};


	}
}