#include "SphereCollider.h"
#include <VrLib/json.hpp>

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

			json SphereCollider::toJson(json &meshes) const
			{
				return json();
			}

		}
	}
}