#include "TransformAttach.h"
#include "Transform.h"
#include "RigidBody.h"
#include "../Scene.h"
#include <VrLib/tien/Node.h>
#include <VrLib/json.hpp>
#include <btBulletDynamicsCommon.h>

namespace vrlib
{
	namespace tien
	{
		namespace components
		{
			TransformAttach::TransformAttach(const vrlib::PositionalDevice &device) : device(device)
			{
				constraint = nullptr;
			}


			TransformAttach::~TransformAttach()
			{
				RigidBody* rigidBody = node->getComponent<RigidBody>();
				if (rigidBody)
				{
					rigidBody->body->setGravity(btVector3(0, -9.8f, 0));
					if (constraint)
						world->removeConstraint(constraint);
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

				glm::vec3 pos(mat * glm::vec4(0, 0, 0, 1));
				glm::quat rot(mat);


				RigidBody* rigidBody = node->getComponent<RigidBody>();
				if (rigidBody && rigidBody->body && rigidBody->getType() == RigidBody::Type::Dynamic)
				{
					node->getComponent<Transform>()->setGlobalRotation(rot);
					btTransform t;
					rigidBody->body->getMotionState()->getWorldTransform(t);
					rigidBody->body->setWorldTransform(t);

					if (constraint == nullptr)
					{ //TODO: make this work better
						rigidBody->body->setGravity(btVector3(0, 0, 0));
						constraint = new btPoint2PointConstraint(*rigidBody->body, btVector3(0,0,0));
						constraint->m_setting.m_impulseClamp = 3;	//0
						constraint->m_setting.m_tau = 0.00001f;			//.3
						constraint->m_setting.m_damping = .3f;		//1
						world = scene.world;
						scene.world->addConstraint(constraint);
					}
					constraint->setPivotB(btVector3(pos.x, pos.y, pos.z));

				}
				else
				{
					node->getComponent<Transform>()->setGlobalPosition(pos);
					node->getComponent<Transform>()->setGlobalRotation(rot);
					if (rigidBody && rigidBody->body)
					{
						btTransform t;
						rigidBody->body->getMotionState()->getWorldTransform(t);
						rigidBody->body->setWorldTransform(t);
					}

				}

			}


		}
	}
}