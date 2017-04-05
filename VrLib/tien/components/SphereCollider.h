#pragma once

#include "Collider.h"

namespace vrlib
{
	namespace tien
	{
		namespace components
		{
			class SphereCollider : public Collider
			{
				virtual physx::PxShape* getShape(physx::PxPhysics* physics, const glm::vec3 &scale) override;
				virtual json toJson(json &meshes) const override;

			};
		}
	}
}