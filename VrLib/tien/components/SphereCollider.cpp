#include "SphereCollider.h"
#include <VrLib/json.hpp>

namespace vrlib
{
	namespace tien
	{
		namespace components
		{
			physx::PxShape* SphereCollider::getShape(physx::PxPhysics* physics, const glm::vec3 &scale)
			{
				return nullptr;
			}

			nlohmann::json SphereCollider::toJson(nlohmann::json &meshes) const
			{
				return nlohmann::json();
			}

		}
	}
}