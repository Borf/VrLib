#pragma once

#include "../Component.h"
#include <VrLib/Device.h>


class btPoint2PointConstraint;

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
			public:
				TransformAttach(const vrlib::PositionalDevice &device);
				~TransformAttach();
				
				void update(float elapsedTime, Scene& scene) override;
			};
		}
	}
}