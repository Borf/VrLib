#include "TransformAttach.h"
#include "Transform.h"
#include "RigidBody.h"
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
			}


			TransformAttach::~TransformAttach()
			{
			}

			void TransformAttach::update(float elapsedTime)
			{
				if (!device.isInitialized())
					return;
				glm::mat4 mat = device.getData();

				glm::vec3 pos(mat * glm::vec4(0, 0, 0, 1));
				glm::quat rot(mat);

				node->getComponent<Transform>()->setGlobalPosition(pos);
				node->getComponent<Transform>()->setGlobalRotation(rot);

				RigidBody* rigidBody = node->getComponent<RigidBody>();
				if (rigidBody && rigidBody->body)
				{
					btTransform transform;
					rigidBody->body->getMotionState()->getWorldTransform(transform);

					transform.setOrigin(btVector3(pos.x, pos.y, pos.z));
					transform.setRotation(btQuaternion(rot.x, rot.y, rot.z, rot.w));

					rigidBody->body->setWorldTransform(transform);
				}
			}


		}
	}
}