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

				Slider(float minValue, float maxValue, float startValue);
				void drag(glm::vec3 intersect); //TODO: use callback
			};
		}
	}
}