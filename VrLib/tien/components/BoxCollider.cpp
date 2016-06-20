#include "BoxCollider.h"
#include "ModelRenderer.h"
#include "Transform.h"
#include "../Node.h"
#include <VrLib/Model.h>
#include <BulletCollision/CollisionShapes/btBoxShape.h>

namespace vrlib
{
	namespace tien
	{
		namespace components
		{
			BoxCollider::BoxCollider(Node* n)
			{
				if (!n)
					return;

				ModelRenderer* model = n->getComponent<ModelRenderer>();
				if (model)
				{
					size = model->model->aabb.bounds[1] - model->model->aabb.bounds[0];
					offset = model->model->aabb.center();
				}
				Transform* transform = n->getComponent<Transform>();
				if (transform)
				{
					size *= transform->scale;
					offset *= transform->scale;
				}
			}

			BoxCollider::BoxCollider(const glm::vec3 &size)
			{
				this->size = size;
			}

			btCollisionShape* BoxCollider::getShape()
			{
				return new btBoxShape(btVector3(size.x / 2.0f, size.y / 2.0f, size.z / 2.0f));
			}

		}
	}
}