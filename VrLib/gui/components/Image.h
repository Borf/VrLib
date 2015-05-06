#pragma once

#include "Component.h"

namespace vrlib
{
	class Texture;
	namespace gui
	{
		namespace components
		{
			class Image : public Component
			{
			public:
				Image(vrlib::Texture* texture) {};
				virtual void draw(const glm::mat4 &parentMatrix) override {};
			};
		}
	}
}