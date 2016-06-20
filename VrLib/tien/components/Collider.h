#pragma once

#include "../Component.h"

#include <glm/glm.hpp>

class btCollisionShape;

namespace vrlib
{
	namespace tien
	{
		namespace components
		{
			class Collider : public Component
			{
			public:
				glm::vec3 offset;
				virtual btCollisionShape* getShape() = 0;

			};
		}
	}
}