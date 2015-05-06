#include "Slider.h"
namespace vrlib
{
	namespace gui
	{
		namespace components
		{

			Slider::Slider(float minValue, float maxValue, float startValue)
			{
				this->min = minValue;
				this->max = maxValue;
				this->value = startValue;
			}

			void Slider::drag(const glm::vec3 &intersect)
			{

			}

		}
	}
}