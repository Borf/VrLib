#include "SphereCollider.h"
#include <VrLib/json.h>

namespace vrlib
{
	namespace tien
	{
		namespace components
		{
			btCollisionShape* SphereCollider::getShape()
			{
				return nullptr;
			}

			json::Value SphereCollider::toJson(json::Value &meshes) const
			{
				return json::Value();
			}

		}
	}
}