#include "RigidBody.h"
#include "Collider.h"
#include "Transform.h"
#include "ModelRenderer.h"
#include "../Node.h"
#include "../Scene.h"
#include <VrLib/Model.h>
#include <VrLib/json.hpp>
#include <VrLib/Log.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>


namespace vrlib
{
	namespace tien
	{
		namespace components
		{

			RigidBody::RigidBody(float mass, Type type)
			{
				this->mass = mass;
				this->type = type;
				actor = nullptr;
				world = nullptr;
			}

			RigidBody::RigidBody(const json & json)
			{
				this->mass = json["mass"];
				this->type = Type::Static; //TODO
				actor = nullptr;
				world = nullptr;
			}


			RigidBody::~RigidBody()
			{
				if (actor && world)
				{
					world->removeActor(*actor);
					actor->release();
				}
			}
			json RigidBody::toJson(json &meshes) const
			{
				json ret;
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

			void RigidBody::init(physx::PxScene* scene)
			{
				this->world = scene;
				Collider* collider = node->getComponent<Collider>();
				Transform* transform = node->getComponent<Transform>();
				ModelRenderer* model = node->getComponent<ModelRenderer>();
				glm::vec3 scale = transform->getGlobalScale();

				if (!collider)
					return;
				physx::PxShape* shape = collider->getShape(&scene->getPhysics(), scale);
				if (!shape)
					return;

				glm::vec3 globalPosition = transform->getGlobalPosition();
				glm::quat globalRotation = transform->getGlobalRotation();// glm::quat(glm::mat3(transform->globalTransform));
				globalRotation = glm::normalize(globalRotation);

				physx::PxTransform tx(physx::PxVec3(globalPosition.x, globalPosition.y, globalPosition.z), physx::PxQuat(globalRotation.x, globalRotation.y, globalRotation.z, globalRotation.w));

				if (mass > 0)
					actor = rigidDynamic = scene->getPhysics().createRigidDynamic(tx);
				else
					actor = scene->getPhysics().createRigidStatic(tx);
				actor->attachShape(*shape);
				if(mass > 0)
					physx::PxRigidBodyExt::updateMassAndInertia(*(physx::PxRigidBody*)actor, 1.0f);

				if(type == Type::Kinematic)
					rigidDynamic->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, true);
				scene->addActor(*actor);
			}

			void RigidBody::updateCollider(physx::PxScene* world)
			{
				std::vector<Collider*> colliders = node->getComponents<Collider>();
				glm::vec3 scale = node->transform->getGlobalScale();

				logger << "Updating collider..." << Log::newline;
				init(world);
				
			}



			void RigidBody::resetMotion()
			{

			}

			void RigidBody::setType(Type newType)
			{
				type = newType;
				rigidDynamic->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, type == Type::Kinematic);
			}






			void RigidBody::buildEditor(EditorBuilder * builder, bool folded)
			{
				builder->addTitle("Rigid Body");
				if (folded)
					return;
				
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