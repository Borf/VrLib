#pragma once

#include <functional>
#include <list>
#include <string>
#include <glm/glm.hpp>
#include <VrLib/math/aabb.h>

namespace vrlib
{
	namespace gui
	{
		namespace components
		{
			class Component
			{
				std::list<std::function<void(void)>> clickHandlers;
			public:
				glm::vec2 position;
				glm::vec2 size;
				std::string name;
				bool hover = false;
				bool mousedown = false;

				void addClickHandler(const std::function<void(void)>& callback);
				void click();
				void setBounds(const glm::vec2 &position, const glm::vec2 &size);
				virtual void draw(const glm::mat4 &parentMatrix) = 0;
				virtual void drag(const glm::vec3 &point) {};
				math::AABB getBoundingBox(const glm::mat4& parentMatrix) const;

			};
		}
	}
}