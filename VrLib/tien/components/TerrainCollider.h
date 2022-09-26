#pragma once

#include "Collider.h"
#include "../Terrain.h"
#include <glm/glm.hpp>

class btHeightfieldTerrainShape;

namespace vrlib
{
	namespace tien
	{
		class Terrain;
		namespace components
		{
			class TerrainCollider : public Collider
			{
				Terrain terrain;
				btHeightfieldTerrainShape* shape;
			public:
				TerrainCollider(Node* n = nullptr);
				virtual nlohmann::json toJson(nlohmann::json &meshes) const override;
				virtual physx::PxShape* getShape(physx::PxPhysics* physics, const glm::vec3 &scale) override;
			};
		}
	}
}