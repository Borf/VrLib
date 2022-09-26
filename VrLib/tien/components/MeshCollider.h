#pragma once

#include "Collider.h"
#include <PxPhysicsAPI.h>

namespace vrlib
{
	namespace tien
	{
		namespace components
		{
			class MeshCollider : public Collider
			{
				bool convex;
			public:
				physx::PxShape* shape;

				MeshCollider(Node* node, bool convex);

				virtual physx::PxShape* getShape(physx::PxPhysics* physics, const glm::vec3 &scale) override;
				virtual nlohmann::json toJson(nlohmann::json &meshes) const override;
				void buildEditor(EditorBuilder * builder, bool folded) override;


				void buildShape(Node* node);
			};
		}
	}
}