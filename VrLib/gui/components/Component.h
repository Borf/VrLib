#pragma once

#include <functional>
#include <glm/glm.hpp>

namespace vrlib
{
	namespace gui
	{
		namespace components
		{
			class Component
			{
			public:
				glm::vec2 position;
				glm::vec2 size;
				std::string name;

				void addClickHandler(const std::function<void(void)>& callback);

				void setBounds(const glm::vec2 &position, const glm::vec2 &size);
				virtual void draw(const glm::mat4 &parentMatrix) = 0;
			};
		}
	}
}