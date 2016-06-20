#pragma once

#include "../Component.h"

#include <btBulletDynamicsCommon.h>
#include <glm/glm.hpp>

namespace vrlib
{
	namespace tien
	{
		namespace components
		{
			class RigidBody : public Component, btMotionState
			{
			public:


				RigidBody(float mass);
				btRigidBody* body;

				float mass;

				void init(btDynamicsWorld* world);
				virtual void getWorldTransform(btTransform & worldTrans) const override;
				virtual void setWorldTransform(const btTransform & worldTrans) override;
			};
		}
	}
}