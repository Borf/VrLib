#include "Label.h"
#include <vrlib/gui/Window.h>
#include <vrlib/gl/shader.h>
#include <vrlib/Font.h>

#include <glm/gtc/matrix_transform.hpp>

namespace vrlib
{
	namespace gui
	{
		namespace components
		{


			Label::Label(const std::string &text, const glm::vec2 &position)
			{
				this->text = text;
				this->position = position;
			}


			void Label::draw(const glm::mat4 &parentMatrix)
			{
				glm::mat4 matrix = glm::translate(parentMatrix, glm::vec3(position + glm::vec2(size.x / 2 - 0.25f * Window::font->getLength("%s", text.c_str()) / 2.0f, 0.125f), mousedown ? 0.03f : vrlib::gui::Window::thickness));
				matrix = glm::scale(matrix, glm::vec3(0.25f, -0.25f, 0.25f));
				vrlib::gui::Window::shader->setUniformMatrix4("modelMatrix", matrix);
				vrlib::gui::Window::shader->setUniformVec4("colorMult", glm::vec4(0.25f, 0.25f, 0.25f, 1));
				Window::font->render("%s", text.c_str());
				vrlib::gui::Window::shader->setUniformVec4("colorMult", glm::vec4(1, 1, 1, 1));
			}
		}
	}
}