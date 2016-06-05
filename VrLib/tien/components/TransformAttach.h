#pragma once

#include "../Component.h"
#include <VrLib/Device.h>

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
			public:
				TransformAttach(const vrlib::PositionalDevice &device);
				~TransformAttach();
				
				void update(float elapsedTime);
			};
		}
	}
}