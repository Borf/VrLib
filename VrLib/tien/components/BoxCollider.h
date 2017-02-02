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
				BoxCollider(const json &json);

				virtual btCollisionShape* getShape() override;

				glm::vec3 getSize() { return size; }
				json toJson(json &meshes) const override;
				void buildEditor(EditorBuilder * builder, bool folded) override;
			};
		}
	}
}