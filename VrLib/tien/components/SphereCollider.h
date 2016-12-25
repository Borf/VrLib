#pragma once

#include "Collider.h"

namespace vrlib
{
	namespace tien
	{
		namespace components
		{
			class SphereCollider : public Collider
			{
				virtual btCollisionShape* getShape() override;
				virtual json::Value toJson(json::Value &meshes) const override;

			};
		}
	}
}