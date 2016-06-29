#include "TerrainCollider.h"
#include "TerrainRenderer.h"
#include "Transform.h"
#include "../Node.h"
#include <VrLib/Model.h>
#include <BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>

namespace vrlib
{
	namespace tien
	{
		namespace components
		{
			TerrainCollider::TerrainCollider(Node* n) : terrain(n->getComponent<components::TerrainRenderer>()->terrain)
			{
				assert(n);
				offset = glm::vec3(terrain.getWidth() / 2.0f, 25.6f/2, terrain.getHeight() / 2.0f);
				Transform* transform = n->getComponent<Transform>();
				if (transform)
					offset *= transform->scale;

				float* data = new float[terrain.getHeight() * terrain.getWidth()];
				memset(data, 0, sizeof(float) * terrain.getHeight() * terrain.getWidth());
				for (int x = 0; x < terrain.getWidth(); x++)
					for (int y = 0; y < terrain.getHeight(); y++)
						data[x + terrain.getWidth() * y] = terrain[x][y];
				shape = new btHeightfieldTerrainShape(terrain.getHeight(), terrain.getWidth(), data, 1, 0, 25.6f, 1, PHY_ScalarType::PHY_FLOAT, true);
			}

			btCollisionShape* TerrainCollider::getShape()
			{
				return shape;
			}

		}
	}
}