#include "TransformAttach.h"
#include "Transform.h"
#include "RigidBody.h"
#include "../Scene.h"
#include <VrLib/tien/Node.h>
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
			}

			void TransformAttach::update(float elapsedTime, Scene& scene)
			{
				if (!device.isInitialized())
					return;
				glm::mat4 mat = device.getData();

				glm::vec3 pos(mat * glm::vec4(0, 0, 0, 1));
				glm::quat rot(mat);


				RigidBody* rigidBody = node->getComponent<RigidBody>();
				if (rigidBody && rigidBody->body && rigidBody->mass > 0)
				{
					node->getComponent<Transform>()->setGlobalRotation(rot);
					if (constraint == nullptr)
					{
						rigidBody->body->setGravity(btVector3(0, 0, 0));
						constraint = new btPoint2PointConstraint(*rigidBody->body, btVector3(0,0,0));
						constraint->m_setting.m_impulseClamp = 0.5f;
						constraint->m_setting.m_tau = 0.001f;

						scene.world->addConstraint(constraint);
					}
					constraint->setPivotB(btVector3(pos.x, pos.y, pos.z));

				}
				else
				{
					node->getComponent<Transform>()->setGlobalPosition(pos);
					node->getComponent<Transform>()->setGlobalRotation(rot);
				}

			}


		}
	}
}