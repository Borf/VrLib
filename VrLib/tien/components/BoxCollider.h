#pragma once

#include "Collider.h"
#include <glm/glm.hpp>

namespace vrlib
{
	namespace tien
	{
		namespace components
		{
			class BoxCollider : public Collider
			{
			public:
				BoxCollider(Node* n = nullptr);
				BoxCollider(const glm::vec3 &size);

				virtual btCollisionShape* getShape() override;


				glm::vec3 size;
			};
		}
	}
}