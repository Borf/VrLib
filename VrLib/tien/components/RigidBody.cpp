#include "RigidBody.h"
#include "Collider.h"
#include "Transform.h"
#include "ModelRenderer.h"
#include "../Node.h"
#include <btBulletDynamicsCommon.h>
#include <VrLib/Model.h>
#include <VrLib/json.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>


namespace vrlib
{
	namespace tien
	{
		namespace components
		{
			btCollisionShape* RigidBody::emptyShape = new btEmptyShape();

			RigidBody::RigidBody(float mass, Type type)
			{
				this->mass = mass;
				this->type = type;
				body = nullptr;
			}

			RigidBody::RigidBody(const json::Value & json)
			{
				this->mass = json["mass"];
				this->type = Type::Static; //TODO
				body = nullptr;
			}


			RigidBody::~RigidBody()
			{
				world->removeRigidBody(body);
				delete body;
			}
			json::Value RigidBody::toJson(json::Value &meshes) const
			{
				json::Value ret;
				ret["type"] = "rigidbody";
				ret["mass"] = mass;
				switch (type)
				{
				case Type::Static:		ret["bodytype"] = "static";		break;
				case Type::Auto:		ret["bodytype"] = "auto";		break;
				case Type::Dynamic:		ret["bodytype"] = "dynamic";	break;
				case Type::Kinematic:	ret["bodytype"] = "kinematic";	break;
				default:
					ret["bodytype"] = "error";
				}
				return ret;
			}

			void RigidBody::init(btDynamicsWorld* world)
			{
				this->world = world;
				Collider* collider = node->getComponent<Collider>();
				Transform* transform = node->getComponent<Transform>();
				ModelRenderer* model = node->getComponent<ModelRenderer>();
				btCollisionShape* shape = collider ? collider->getShape() : emptyShape;

				glm::vec3 scale = transform->getGlobalScale();

				shape->setLocalScaling(btVector3(scale.x, scale.y, scale.z));
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
				glm::vec3 scale = node->transform->getGlobalScale();

				if (colliders.size() == 1)
				{
					colliders[0]->getShape()->setLocalScaling(btVector3(scale.x, scale.y, scale.z));
					body->setCollisionShape(colliders[0]->getShape());
					btVector3 inertia(0,0,0);
					if(mass > 0)
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
					compound->setLocalScaling(btVector3(scale.x, scale.y, scale.z));
					btVector3 inertia(0, 0, 0);
					if (mass > 0)
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

			void RigidBody::setType(Type newType)
			{
				world->removeRigidBody(body);
				if (newType == Type::Dynamic)
				{
					body->setCollisionFlags(body->getCollisionFlags() & ~btCollisionObject::CF_KINEMATIC_OBJECT);
				}
				else if (newType == Type::Static || newType == Type::Kinematic)
				{
					body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
				}
				world->addRigidBody(body);

				type = newType;
			}




			void RigidBody::getWorldTransform(btTransform & worldTrans) const
			{
				Transform* transform = node->getComponent<Transform>();
				ModelRenderer* model = node->getComponent<ModelRenderer>();
#if 1
				glm::mat4 mat(transform->globalTransform);
				mat = glm::scale(mat, 1.0f / transform->getGlobalScale());
				if(node->getComponent<Collider>())
					mat = glm::translate(mat, node->getComponent<Collider>()->offset);

				worldTrans.setFromOpenGLMatrix(glm::value_ptr(mat));
#else
				glm::vec3 position = transform->getGlobalPosition();
				position += transform->rotation * (node->getComponent<Collider>() ? node->getComponent<Collider>()->offset : glm::vec3(0, 0, 0));

				worldTrans.setOrigin(btVector3(position.x, position.y, position.z));
				worldTrans.setRotation(btQuaternion(transform->rotation.x, transform->rotation.y, transform->rotation.z, transform->rotation.w));
#endif

			}
			void RigidBody::setWorldTransform(const btTransform & worldTrans)
			{
				if (!node->getComponent<Collider>())
					return;
				Transform* transform = node->getComponent<Transform>();
				ModelRenderer* model = node->getComponent<ModelRenderer>();

				//todo: check
				transform->setGlobalPosition(glm::vec3(worldTrans.getOrigin().x(), worldTrans.getOrigin().y(), worldTrans.getOrigin().z()), false);
				//transform->position = (glm::vec3(worldTrans.getOrigin().x(), worldTrans.getOrigin().y(), worldTrans.getOrigin().z()));
				transform->rotation = glm::quat(worldTrans.getRotation().w(), worldTrans.getRotation().x(), worldTrans.getRotation().y(), worldTrans.getRotation().z());
				transform->position -= transform->rotation * node->getComponent<Collider>()->offset;

				float f[16];
				worldTrans.getOpenGLMatrix(f);
				transform->globalTransform = glm::make_mat4(f);

			}




			void RigidBody::buildEditor(EditorBuilder * builder)
			{
				builder->addTitle("Rigid Body");

				builder->beginGroup("Mass");
				builder->addTextBox(builder->toString(mass), [this](const std::string & newValue) { mass = (float)atof(newValue.c_str());  });
				builder->endGroup();

				builder->beginGroup("Type");
				builder->addComboBox("static", { "static", "dynamic", "kinematic" }, [](const std::string &newValue) {});
				builder->endGroup();
				builder->endGroup();

			}

		}
	}
}