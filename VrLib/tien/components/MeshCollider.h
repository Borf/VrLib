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
				bool convex;
			public:
				btCollisionShape* shape;

				MeshCollider(Node* node, bool convex);

				virtual btCollisionShape* getShape() override;
				virtual json toJson(json &meshes) const override;
				void buildEditor(EditorBuilder * builder, bool folded) override;


				void buildShape(Node* node);
			};
		}
	}
}