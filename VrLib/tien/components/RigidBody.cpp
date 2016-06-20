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


			RigidBody::~RigidBody()
			{
				world->removeRigidBody(body);
				delete body;
			}

			void RigidBody::init(btDynamicsWorld* world)
			{
				this->world = world;
				Collider* collider = node->getComponent<Collider>();
				Transform* transform = node->getComponent<Transform>();
				ModelRenderer* model = node->getComponent<ModelRenderer>();
				btCollisionShape* shape = collider->getShape();

				btVector3 fallInertia;
				shape->calculateLocalInertia(mass, fallInertia);
				btRigidBody::btRigidBodyConstructionInfo cInfo(mass, this, shape, fallInertia);
				body = new btRigidBody(cInfo);

				body->setFriction(1.0f);
				body->setRestitution(0.0f);
				
				if(mass == 0)
					body->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);

				world->addRigidBody(body);
				body->setActivationState(DISABLE_DEACTIVATION);
				body->setActivationState(ACTIVE_TAG);
			}




			void RigidBody::getWorldTransform(btTransform & worldTrans) const
			{
				Transform* transform = node->getComponent<Transform>();
				ModelRenderer* model = node->getComponent<ModelRenderer>();
				
				glm::vec3 position = transform->position;
				position += transform->rotation * node->getComponent<Collider>()->offset;

				worldTrans.setOrigin(btVector3(position.x, position.y, position.z));
				worldTrans.setRotation(btQuaternion(transform->rotation.x, transform->rotation.y, transform->rotation.z, transform->rotation.w));
			}
			void RigidBody::setWorldTransform(const btTransform & worldTrans)
			{
				Transform* transform = node->getComponent<Transform>();
				ModelRenderer* model = node->getComponent<ModelRenderer>();

				transform->position = glm::vec3(worldTrans.getOrigin().x(), worldTrans.getOrigin().y(), worldTrans.getOrigin().z());
				transform->rotation = glm::quat(worldTrans.getRotation().w(), worldTrans.getRotation().x(), worldTrans.getRotation().y(), worldTrans.getRotation().z());
				transform->position -= transform->rotation * node->getComponent<Collider>()->offset;
			}
		}
	}
}