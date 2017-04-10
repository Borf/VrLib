#include "TransformAttach.h"
#include "Transform.h"
#include "RigidBody.h"
#include "../Scene.h"
#include <VrLib/tien/Node.h>
#include <VrLib/json.hpp>

namespace vrlib
{
	namespace tien
	{
		namespace components
		{
			TransformAttach::TransformAttach(const vrlib::PositionalDevice &device) : device(device)
			{
			}


			TransformAttach::~TransformAttach()
			{
				RigidBody* rigidBody = node->getComponent<RigidBody>();
				if (rigidBody)
				{
				}
			}

			json TransformAttach::toJson(json &meshes) const
			{
				json ret;
				ret["type"] = "transformattach";
				ret["attach"] = device.name;
				return ret;
			}

			void TransformAttach::postUpdate(Scene& scene)
			{
				if (!device.isInitialized())
					return;


				glm::mat4 mat = device.getData();
				if (scene.cameraNode)
					mat = scene.cameraNode->transform->globalTransform * mat;
				mat *= offset;

				glm::vec3 pos(mat * glm::vec4(0, 0, 0, 1));
				glm::quat rot(mat);
				RigidBody* rigidBody = node->getComponent<RigidBody>();

				if (rigidBody && rigidBody->actor)
				{
					physx::PxTransform tx(physx::PxVec3(pos.x, pos.y, pos.z), physx::PxQuat(rot.x, rot.y, rot.z, rot.w));
					if(rigidBody->rigidDynamic && rigidBody->rigidDynamic->getRigidBodyFlags().isSet(physx::PxRigidBodyFlag::eKINEMATIC))
						rigidBody->rigidDynamic->setKinematicTarget(tx);
					else
						rigidBody->actor->setGlobalPose(tx, true);
				}
				else
				{
					node->getComponent<Transform>()->setGlobalPosition(pos);
					node->getComponent<Transform>()->setGlobalRotation(rot);
				}
				
				node->fortree([](Node* n)
				{
					n->transform->buildTransform((n->parent && n->parent->transform) ? n->parent->transform->globalTransform : glm::mat4());
				});

			}


		}
	}
}