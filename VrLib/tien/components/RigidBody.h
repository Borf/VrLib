#pragma once

#include "../Component.h"

class btRigidBody;
class btDynamicsWorld;

namespace vrlib
{
	namespace tien
	{
		namespace components
		{
			class RigidBody : public Component
			{
			public:
				RigidBody(float mass);
				btRigidBody* body;

				float mass;

				void init(btDynamicsWorld* world);
				virtual void update(float elapsedTime) override;
			};
		}
	}
}