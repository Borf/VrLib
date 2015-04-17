#pragma once

#include "Component.h"

namespace vrlib
{
	namespace gui
	{
		namespace components
		{
			class Slider : public Component
			{
			protected:
				float min, max;
			public:
				float value;

				Slider(int minValue, int maxValue, int startValue);
				void drag(glm::vec3 intersect); //TODO: use callback
			};
		}
	}
}