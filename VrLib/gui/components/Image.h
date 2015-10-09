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
				vrlib::Texture* texture;
			public:
				Image(vrlib::Texture* texture) { this->texture = texture; };
				virtual void draw(const glm::mat4 &parentMatrix) override;
			};
		}
	}
}