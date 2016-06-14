#include "RigidBody.h"
#include "Collider.h"
#include "Transform.h"
#include "ModelRenderer.h"
#include "../Node.h"
#include <btBulletDynamicsCommon.h>
#include <VrLib/Model.h>


namespace vrlib
{
	namespace tien
	{
		namespace components
		{
			RigidBody::RigidBody(float mass)
			{
				this->mass = mass;
				body = nullptr;
			}
			void RigidBody::init(btDynamicsWorld* world)
			{
				Collider* collider = node->getComponent<Collider>();
				Transform* transform = node->getComponent<Transform>();
				ModelRenderer* model = node->getComponent<ModelRenderer>();
				btCollisionShape* shape = collider->getShape();

				glm::vec3 position = transform->position;
				if (model && model->model)
					position += model->model->aabb.center();

				btTransform groundTransform;
				groundTransform.setIdentity();
				groundTransform.setOrigin(btVector3(position.x, position.y, position.z));
				btVector3 fallInertia;
				shape->calculateLocalInertia(mass, fallInertia);
				btDefaultMotionState* motionState = new btDefaultMotionState(groundTransform);

				btRigidBody::btRigidBodyConstructionInfo cInfo(mass, motionState, shape, fallInertia);
				body = new btRigidBody(cInfo);

				world->addRigidBody(body);
				body->setActivationState(DISABLE_DEACTIVATION);
				body->setActivationState(ACTIVE_TAG);


			}
			void RigidBody::update(float elapsedTime)
			{
				if (!body)
					return;
				Transform* transform = node->getComponent<Transform>();

				btTransform worldTransform;
				body->getMotionState()->getWorldTransform(worldTransform);

				transform->position = glm::vec3(worldTransform.getOrigin().x(), worldTransform.getOrigin().y(), worldTransform.getOrigin().z());
				transform->rotation = glm::quat(worldTransform.getRotation().getW(), worldTransform.getRotation().getX(), worldTransform.getRotation().getY(), worldTransform.getRotation().getZ());
			}
		}
	}
}