#include "TerrainCollider.h"
#include "TerrainRenderer.h"
#include "Transform.h"
#include "../Node.h"
#include <VrLib/Model.h>
#include <string.h>
#include <VrLib/json.hpp>

namespace vrlib
{
	namespace tien
	{
		namespace components
		{
			TerrainCollider::TerrainCollider(Node* n) : terrain(n->getComponent<components::TerrainRenderer>()->terrain)
			{
				assert(n);
				offset = glm::vec3(terrain.getWidth() / 2.0f, terrain.getStretch()/2.0f, terrain.getHeight() / 2.0f);
				Transform* transform = n->getComponent<Transform>();
				if (transform)
					offset *= transform->scale;

		/*		float* data = new float[terrain.getHeight() * terrain.getWidth()];
				memset(data, 0, sizeof(float) * terrain.getHeight() * terrain.getWidth());
				for (int x = 0; x < terrain.getWidth(); x++)
					for (int y = 0; y < terrain.getHeight(); y++)
						data[x + terrain.getWidth() * y] = terrain[x][y];
				shape = new btHeightfieldTerrainShape(terrain.getWidth(), terrain.getHeight(), data, 1, 0, terrain.getStretch(), 1, PHY_ScalarType::PHY_FLOAT, true);*/
			}

			json TerrainCollider::toJson(json &meshes) const
			{
				json ret;
				ret["type"] = "collider";
				ret["collider"] = "terrain";
				for (int i = 0; i < 3; i++)
					ret["offset"].push_back(offset[i]);
				return ret;
			}

			physx::PxShape* TerrainCollider::getShape(physx::PxPhysics* physics, const glm::vec3 &scale)
			{
				return nullptr;
			}

		}
	}
}