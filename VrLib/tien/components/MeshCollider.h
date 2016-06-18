#pragma once

#include "Collider.h"

namespace vrlib
{
	namespace tien
	{
		namespace components
		{
			class MeshCollider : public Collider
			{
			public:
				virtual btCollisionShape* getShape() override;
			};
		}
	}
}