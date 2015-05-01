#include "Button.h"

#include <glm/gtc/matrix_transform.hpp>

#include <vrlib/gui/Window.h>
#include <vrlib/Model.h>
#include <vrlib/gl/shader.h>

namespace vrlib
{
	namespace gui
	{
		namespace components
		{
			Button::Button(const std::string &text, const glm::vec2 &position /*= glm::vec2(0,0)*/, std::function<void()> callback /*= nullptr*/)
			{
				this->text = text;
				this->position = position;
				this->size = glm::vec2(1, 0.25f);
			}

			void Button::draw(const glm::mat4 &parentMatrix)
			{
			/*	glm::mat4 matrix = glm::translate(parentMatrix, glm::vec3(position, 0));
				matrix = glm::scale(matrix, glm::vec3(size, 1));

				vrlib::gui::Window::panelModel->draw([&matrix](const glm::mat4& mat)
				{
					vrlib::gui::Window::shader->setUniformMatrix4("modelMatrix", mat * matrix);
				});*/
			}


		}
	}
}