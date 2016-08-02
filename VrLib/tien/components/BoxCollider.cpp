#include "BoxCollider.h"
#include "BoxCollider.h"
#include "ModelRenderer.h"
#include "Transform.h"
#include "../Node.h"
#include <VrLib/Model.h>
#include <VrLib/json.h>
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
//					size *= transform->scale;
					offset *= transform->scale;
				}
				shape = new btBoxShape(btVector3(size.x / 2.0f, size.y / 2.0f, size.z / 2.0f));
			}

			BoxCollider::BoxCollider(const glm::vec3 &size)
			{
				this->size = size;
				shape = new btBoxShape(btVector3(size.x / 2.0f, size.y / 2.0f, size.z / 2.0f));
			}

			btCollisionShape* BoxCollider::getShape()
			{
				return shape;
			}

			json::Value BoxCollider::toJson() const
			{
				json::Value ret;
				ret["type"] = "collider";
				ret["collider"] = "box";
				return ret;
			}

		}
	}
}