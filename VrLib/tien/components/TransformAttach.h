#pragma once

#include "../Component.h"
#include <VrLib/Device.h>


class btPoint2PointConstraint;
class btDiscreteDynamicsWorld;

namespace vrlib
{
	class PositionalDevice;

	namespace tien
	{
		class Renderer;

		namespace components
		{
			class TransformAttach : public Component
			{
				const vrlib::PositionalDevice &device;
				btPoint2PointConstraint* constraint;
				btDiscreteDynamicsWorld* world;
			public:
				TransformAttach(const vrlib::PositionalDevice &device);
				~TransformAttach();
				json toJson(json &meshes) const override;

				void postUpdate(Scene& scene) override;
			};
		}
	}
}