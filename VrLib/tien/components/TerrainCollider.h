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
				virtual json toJson(json &meshes) const override;
				virtual btCollisionShape* getShape() override;
			};
		}
	}
}