#pragma once

#include "Component.h"

#include <glm/glm.hpp>

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
				void drag(const glm::vec3 &intersect); //TODO: use callback
				virtual void draw(const glm::mat4 &parentMatrix) override {};
			};
		}
	}
}