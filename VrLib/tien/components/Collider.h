#pragma once

#include "../Component.h"
#include <glm/glm.hpp>

namespace physx
{
	class PxShape;
	class PxPhysics;
}

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
				virtual physx::PxShape* getShape(physx::PxPhysics* physics, const glm::vec3 &scale) = 0;

			};
		}
	}
}