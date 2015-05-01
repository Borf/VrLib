#include "Panel.h"

#include <glm/gtc/matrix_transform.hpp>

#include <VrLib/gui/Window.h>
#include <VrLib/gl/shader.h>
#include <VrLib/Model.h>

namespace vrlib
{
	namespace gui
	{
		namespace components
		{
			Panel::Panel()
			{

			}

			void Panel::draw(const glm::mat4 &parentMatrix)
			{
				glm::mat4 matrix = glm::translate(parentMatrix, glm::vec3(position, 0));
				matrix = glm::scale(matrix, glm::vec3(((Component*)this)->size, 1));


				vrlib::gui::Window::panelModel->draw([this, &matrix](const glm::mat4& mat)
				{
					vrlib::gui::Window::shader->setUniformMatrix4("modelMatrix", matrix * mat);
				});


				ContainerComponent::draw(glm::translate(parentMatrix, glm::vec3(position, Window::thickness)));
			}

		}
	}
}
