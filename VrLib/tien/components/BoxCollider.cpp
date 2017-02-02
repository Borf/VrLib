#include "BoxCollider.h"
#include "BoxCollider.h"
#include "ModelRenderer.h"
#include "AnimatedModelRenderer.h"
#include "Transform.h"
#include "../Node.h"
#include <VrLib/Model.h>
#include <VrLib/json.hpp>
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

				Model* model = nullptr;

				if (n->getComponent<ModelRenderer>())
					model = n->getComponent<ModelRenderer>()->model;
				if (n->getComponent<AnimatedModelRenderer>())
					model = n->getComponent<AnimatedModelRenderer>()->model;

				if (model)
				{
					size = model->aabb.bounds[1] - model->aabb.bounds[0];
					offset = model->aabb.center();
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

			BoxCollider* BoxCollider::fromJson(const json & json)
			{
				BoxCollider* ret = new BoxCollider();
				for (int i = 0; i < 3; i++)
					ret->offset[i] = json["offset"][i];
				for (int i = 0; i < 3; i++)
					ret->size[i] = json["size"][i];
				ret->shape = new btBoxShape(btVector3(ret->size.x / 2.0f, ret->size.y / 2.0f, ret->size.z / 2.0f));
				return ret;
			}

			btCollisionShape* BoxCollider::getShape()
			{
				return shape;
			}

			json BoxCollider::toJson(json &meshes) const
			{
				json ret;
				ret["type"] = "collider";
				ret["collider"] = "box";
				for (int i = 0; i < 3; i++)
					ret["offset"].push_back(offset[i]);
				for (int i = 0; i < 3; i++)
					ret["size"].push_back(size[i]);
				return ret;
			}


			void BoxCollider::buildEditor(EditorBuilder * builder, bool folded)
			{
				builder->addTitle("Box Collider");
				if (folded)
					return;
				builder->beginGroup("Size", false);
				for(int i = 0; i < 3; i++)
					builder->addTextBox(builder->toString(size[i]), [this, i](const std::string & newValue) 
					{ 
						size[i] = (float)atof(newValue.c_str());  
						shape->setImplicitShapeDimensions(btVector3(size.x/2, size.y/2, size.z/2));
					});
				builder->endGroup();

				builder->beginGroup("offset", false);
				for (int i = 0; i < 3; i++)
					builder->addTextBox(builder->toString(offset[i]), [this, i](const std::string & newValue) { offset[i] = (float)atof(newValue.c_str());  });
				builder->endGroup();

			}


		}
	}
}