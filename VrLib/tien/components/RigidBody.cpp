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
			btCollisionShape* RigidBody::emptyShape = new btEmptyShape();;

			RigidBody::RigidBody(float mass, Type type)
			{
				this->mass = mass;
				this->type = type;
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
				btCollisionShape* shape = collider ? collider->getShape() : emptyShape;

				shape->setLocalScaling(btVector3(transform->scale.x, transform->scale.y, transform->scale.z));
				btVector3 fallInertia(0,0,0);
				if(shape != emptyShape)
					shape->calculateLocalInertia(mass, fallInertia);
				btRigidBody::btRigidBodyConstructionInfo cInfo(mass, this, shape, fallInertia);
				body = new btRigidBody(cInfo);

				body->setFriction(1.0f);
				body->setRestitution(0.0f);
				body->setUserPointer(node);
				if(mass == 0)
					body->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);

				world->addRigidBody(body);
				body->setActivationState(DISABLE_DEACTIVATION);
				body->setActivationState(ACTIVE_TAG);
			}

			void RigidBody::updateCollider(btDynamicsWorld* world)
			{
				std::vector<Collider*> colliders = node->getComponents<Collider>();
				world->removeRigidBody(body);

				if (colliders.size() == 1)
				{
					colliders[0]->getShape()->setLocalScaling(btVector3(node->transform->scale.x, node->transform->scale.y, node->transform->scale.z));
					body->setCollisionShape(colliders[0]->getShape());
					btVector3 inertia;
					body->getCollisionShape()->calculateLocalInertia(mass, inertia);
					body->setMassProps(mass, inertia);
					resetMotion();
				}
				else if (colliders.size() > 1)
				{
					btCompoundShape* compound = new btCompoundShape(); // TODO: this leaks !
					for(auto c : colliders)
						compound->addChildShape(btTransform(), c->getShape());
					body->setCollisionShape(compound);
					compound->setLocalScaling(btVector3(node->transform->scale.x, node->transform->scale.y, node->transform->scale.z));
					btVector3 inertia;
					compound->calculateLocalInertia(mass, inertia);
					body->setMassProps(mass, inertia);
					resetMotion();
				}
				else
					body->setCollisionShape(emptyShape);
				world->addRigidBody(body);
			}



			void RigidBody::resetMotion()
			{
				body->setLinearVelocity(btVector3(0, 0, 0));
				body->setAngularVelocity(btVector3(0, 0, 0));
			}






			void RigidBody::getWorldTransform(btTransform & worldTrans) const
			{
				Transform* transform = node->getComponent<Transform>();
				ModelRenderer* model = node->getComponent<ModelRenderer>();
				
				glm::vec3 position = transform->position;
				position += transform->rotation * (node->getComponent<Collider>() ? node->getComponent<Collider>()->offset : glm::vec3(0,0,0));

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