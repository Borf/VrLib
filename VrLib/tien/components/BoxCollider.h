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
				glm::vec3 size;
			public:
				static BoxCollider* fromJson(const nlohmann::json &json, Scene *scene);
				BoxCollider(Node* n = nullptr);
				BoxCollider(const glm::vec3 &size);

				virtual physx::PxShape* getShape(physx::PxPhysics* physics, const glm::vec3 &scale) override;

				glm::vec3 getSize() { return size; }
				nlohmann::json toJson(nlohmann::json &meshes) const override;
				void buildEditor(EditorBuilder * builder, bool folded) override;
				virtual void drawDebug() override;
			};
		}
	}
}