#include "Component.h"
#include <VrLib/gui/Window.h>
#include <glm/gtc/matrix_transform.hpp>
#include "..\..\tien\Component.h"

namespace vrlib
{
	namespace gui
	{
		namespace components
		{


			void Component::addClickHandler(const std::function<void(void)>& callback)
			{
				clickHandlers.push_back(callback);
			}

			void Component::setBounds(const glm::vec2 &position, const glm::vec2 &size)
			{
				this->position = position;
				this->size = size;
			}

			math::AABB Component::getBoundingBox(const glm::mat4& parentMatrix) const
			{
				glm::mat4 matrix = glm::translate(parentMatrix, glm::vec3(position, 0));
				matrix = glm::scale(matrix, glm::vec3(((Component*)this)->size, 1));

				glm::vec4 p1(0, 0, 0, 1);
				glm::vec4 p2(1, 1, vrlib::gui::Window::thickness, 1);

				return math::AABB(glm::vec3(matrix * p1), glm::vec3(matrix * p2));

			}

			void Component::click()
			{
				for (const std::function<void(void)>& handler : clickHandlers)
					handler();
			}

		}
	}
}