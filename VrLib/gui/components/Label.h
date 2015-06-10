#pragma once

#include "Component.h"

#include <string>

namespace vrlib
{
	namespace gui
	{
		namespace components
		{
			class Label : public Component
			{
			public:
				std::string text;

				Label(const std::string &text, const glm::vec2 &position = glm::vec2(0,0));
				virtual void draw(const glm::mat4 &parentMatrix) override;
			};
		}
	}
}