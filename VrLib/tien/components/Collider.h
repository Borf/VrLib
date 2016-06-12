#pragma once

#include "../Component.h"

class btCollisionShape;

namespace vrlib
{
	namespace tien
	{
		namespace components
		{
			class Collider : public Component
			{
			public:
				virtual btCollisionShape* getShape() = 0;

			};
		}
	}
}