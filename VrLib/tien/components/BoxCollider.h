#pragma once

#include "Collider.h"
#include <glm/glm.hpp>

class btBoxShape;

namespace vrlib
{
	namespace tien
	{
		namespace components
		{
			class BoxCollider : public Collider
			{
				btBoxShape* shape;
				glm::vec3 size;
			public:
				BoxCollider(Node* n = nullptr);
				BoxCollider(const glm::vec3 &size);

				virtual btCollisionShape* getShape() override;

				glm::vec3 getSize() { return size; }

			};
		}
	}
}