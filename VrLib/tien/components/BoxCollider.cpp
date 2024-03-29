#include "BoxCollider.h"
#include "BoxCollider.h"
#include "ModelRenderer.h"
#include "AnimatedModelRenderer.h"
#include "Transform.h"
#include "../Node.h"
#include "../Scene.h"
#include <VrLib/Model.h>
#include <VrLib/json.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
					//offset *= transform->getGlobalScale();
				}
			}

			BoxCollider::BoxCollider(const glm::vec3 &size)
			{
				this->size = size;
			}

			BoxCollider* BoxCollider::fromJson(const nlohmann::json & json, Scene* scene)
			{
				BoxCollider* ret = new BoxCollider();
				for (int i = 0; i < 3; i++)
					ret->offset[i] = json["offset"][i];
				for (int i = 0; i < 3; i++)
					ret->size[i] = json["size"][i];
				return ret;
			}

			physx::PxShape* BoxCollider::getShape(physx::PxPhysics* physics, const glm::vec3 &scale)
			{
				physx::PxShape * shape = physics->createShape(physx::PxBoxGeometry(size.x / 2 * scale.x, size.y / 2 * scale.x, size.z / 2 * scale.z), 
					*node->getScene().gMaterial);
				shape->setLocalPose(physx::PxTransform(physx::PxVec3(offset.x * scale.x, offset.y * scale.y, offset.z * scale.z)));
				return shape;
			}

			nlohmann::json BoxCollider::toJson(nlohmann::json &meshes) const
			{
				nlohmann::json ret;
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
					builder->addFloatBox(size[i], -1000, 1000, [this, i](float newValue) { size[i] = newValue; } );
				builder->endGroup();

				builder->beginGroup("offset", false);
				for (int i = 0; i < 3; i++)
					builder->addFloatBox(offset[i], -1000, 1000, [this, i](float newValue) { offset[i] = newValue;  });
				builder->endGroup();

			}

			void BoxCollider::drawDebug()
			{
				glPushMatrix();
				glMultMatrixf(glm::value_ptr(glm::translate(node->transform->globalTransform, this->offset)));
				glBegin(GL_QUADS);
				glVertex3f(-size.x / 2,	-size.y / 2,	size.z / 2);
				glVertex3f(size.x / 2,	-size.y / 2,	size.z / 2);
				glVertex3f(size.x / 2,	size.y / 2,		size.z / 2);
				glVertex3f(-size.x / 2, size.y / 2,		size.z / 2);

				glVertex3f(-size.x / 2, size.y / 2,		-size.z / 2);
				glVertex3f(size.x / 2,	size.y / 2,		-size.z / 2);
				glVertex3f(size.x / 2,	-size.y / 2,	-size.z / 2);
				glVertex3f(-size.x / 2, -size.y / 2,	-size.z / 2);

				glVertex3f(-size.x / 2, size.y / 2,		size.z / 2);
				glVertex3f(size.x / 2,	size.y / 2,		size.z / 2);
				glVertex3f(size.x / 2,	size.y / 2,		-size.z / 2);
				glVertex3f(-size.x / 2, size.y / 2,		-size.z / 2);

				glVertex3f(-size.x / 2, -size.y / 2,	-size.z / 2);
				glVertex3f(size.x / 2,	-size.y / 2,	-size.z / 2);
				glVertex3f(size.x / 2,	-size.y / 2,	size.z / 2);
				glVertex3f(-size.x / 2, -size.y / 2,	size.z / 2);


				glVertex3f(-size.x / 2, -size.y / 2,	size.z / 2);
				glVertex3f(-size.x / 2,	size.y / 2,		size.z / 2);
				glVertex3f(-size.x / 2,	size.y / 2,		-size.z / 2);
				glVertex3f(-size.x / 2, -size.y / 2,	-size.z / 2);

				glVertex3f(size.x / 2, -size.y / 2,		-size.z / 2);
				glVertex3f(size.x / 2,	size.y / 2,		-size.z / 2);
				glVertex3f(size.x / 2,	size.y / 2,		size.z / 2);
				glVertex3f(size.x / 2, -size.y / 2,		size.z / 2);

				glEnd();
				glPopMatrix();
			}


		}
	}
}