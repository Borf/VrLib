#pragma once

#include "Collider.h"
#include "../Terrain.h"
#include <glm/glm.hpp>

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
			public:
				TerrainCollider(Node* n = nullptr);

				virtual btCollisionShape* getShape() override;
			};
		}
	}
}