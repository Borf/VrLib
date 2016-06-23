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
			TerrainCollider::TerrainCollider(Node* n)
			{
				if (!n)
					return;

				terrain = n->getComponent<components::TerrainRenderer>()->terrain;
				if (terrain)
				{

				}
			}

		

			btCollisionShape* TerrainCollider::getShape()
			{
				return nullptr;
			}

		}
	}
}