#include "CheckBox.h"
#include <VrLib/Model.h>
#include <VrLib/gl/shader.h>
#include <VrLib/gl/Vertex.h>
#include <VrLib/gui/Window.h>
#include <glm/gtc/matrix_transform.hpp>

namespace vrlib
{
	namespace gui
	{
		namespace components
		{
			CheckBox::Models CheckBox::models;

			CheckBox::CheckBox(bool initialValue, const glm::vec2 &position, const std::function<void()> &callback /*= nullptr*/)
			{
				if (!models.on)
				{
					models.on = Model::getModel<gl::VertexP3N3T2>("data/vrlib/checkbox_on.dae");
					models.off = Model::getModel<gl::VertexP3N3T2>("data/vrlib/checkbox_off.dae");
				}
				this->value = initialValue;

				this->position = position;
				this->size = glm::vec2(0.2f, 0.2f);

				addClickHandler([this, callback]()
				{
					this->value = !this->value;
					if (callback)
						addClickHandler(callback);
				});


			}


			void CheckBox::draw(const glm::mat4 &parentMatrix)
			{
				glm::mat4 matrix = glm::translate(parentMatrix, glm::vec3(position, 0));
				matrix = glm::scale(matrix, glm::vec3(((Component*)this)->size, glm::max(((Component*)this)->size.x, ((Component*)this)->size.y)));

				if (hover)
					vrlib::gui::Window::shader->setUniformVec4("colorMult", glm::vec4(0.75f, 0.75f, 0.75f, 1));

				vrlib::Model* model = value ? models.on : models.off;

				model->draw([this, &matrix](const glm::mat4& mat)
				{
					vrlib::gui::Window::shader->setUniformMatrix4("modelMatrix", matrix * mat);
				});
				vrlib::gui::Window::shader->setUniformVec4("colorMult", glm::vec4(1, 1, 1, 1));
			}
		}
	}
}