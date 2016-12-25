#include "MeshCollider.h"
#include <VrLib/json.h>

namespace vrlib
{
	namespace tien
	{
		namespace components
		{
			btCollisionShape* MeshCollider::getShape()
			{
				return nullptr;
			}

			json::Value MeshCollider::toJson(json::Value & meshes) const
			{
				return json::Value();
			}

		}
	}
}