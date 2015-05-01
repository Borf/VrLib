#pragma once

#include <functional>
#include <string>

#include "Component.h"

namespace vrlib
{
	namespace gui
	{
		namespace components
		{
			class Button : public Component
			{
			public:
				std::string text;

				Button(const std::string &text, const glm::vec2 &position = glm::vec2(0,0), std::function<void()> callback = nullptr);;
				virtual void draw(const glm::mat4 &parentMatrix) override ;
			};
		}
	}
}