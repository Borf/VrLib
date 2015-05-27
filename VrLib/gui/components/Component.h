#pragma once

#include <functional>
#include <glm/glm.hpp>
#include <vrlib/math/aabb.h>

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
				bool hover = false;

				void addClickHandler(const std::function<void(void)>& callback);
				void setBounds(const glm::vec2 &position, const glm::vec2 &size);
				virtual void draw(const glm::mat4 &parentMatrix) = 0;

				math::AABB getBoundingBox(const glm::mat4& parentMatrix) const;

			};
		}
	}
}