#include "Slider.h"

#include <VrLib/gui/Window.h>
#include <VrLib/gl/shader.h>
#include <VrLib/gl/Vertex.h>
#include <VrLib/Model.h>


#include <glm/gtc/matrix_transform.hpp>

namespace vrlib
{
	namespace gui
	{
		namespace components
		{
			Model* Slider::backModel;
			Model* Slider::sliderModel;

			Slider::Slider(float minValue, float maxValue, float startValue)
			{
				if (!backModel)
					backModel = Model::getModel<gl::VertexP3N3T2>("data/vrlib/panel.dae");
				if (!sliderModel)
					sliderModel = Model::getModel<gl::VertexP3N3T2>("data/vrlib/panel.dae");

				this->min = minValue;
				this->max = maxValue;
				this->value = startValue;
			}

			void Slider::drag(const glm::vec3 &intersect)
			{
				float fac = (intersect.x - position.x) / size.x;
				this->value = min + fac * (this->max - this->min);
			}

			void Slider::draw(const glm::mat4 &parentMatrix)
			{
				if (hover)
					vrlib::gui::Window::shader->setUniformVec4("colorMult", glm::vec4(0.5f, 0.5f, 0.5f, 1));

				glm::mat4 matrix = glm::translate(parentMatrix, glm::vec3(position + glm::vec2(0, size.y / 2 - 0.025f), mousedown ? -0.01f : 0.0f));
				matrix = glm::scale(matrix, glm::vec3(size.x, 0.05f, 1));
				backModel->draw([this, &matrix](const glm::mat4& mat)
				{
					vrlib::gui::Window::shader->setUniformMatrix4("modelMatrix", matrix * mat);
				});

				matrix = glm::translate(parentMatrix, glm::vec3(position + glm::vec2((size.x - 0.05f) * (value / (max - min)) , 0), Window::thickness + (mousedown ? -0.01f : 0.0f)));
				matrix = glm::scale(matrix, glm::vec3(0.05f, size.y, 1));
				sliderModel->draw([this, &matrix](const glm::mat4& mat)
				{
					vrlib::gui::Window::shader->setUniformMatrix4("modelMatrix", matrix * mat);
				});



				if (hover)
					vrlib::gui::Window::shader->setUniformVec4("colorMult", glm::vec4(1.0f, 1.0f, 1.0f, 1));


			}

		}
	}
}