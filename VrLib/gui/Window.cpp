#include "Window.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <VrLib/Model.h>
#include <VrLib/gl/Vertex.h>
#include <VrLib/gl/shader.h>
#include <VrLib/gui/components/Panel.h>
#include <VrLib/Font.h>


namespace vrlib
{
	class Model;
	namespace gl { class ShaderProgram;  }


	namespace gui
	{
		vrlib::Model* Window::panelModel = NULL;
		vrlib::gl::ShaderProgram* Window::shader = NULL;
		vrlib::Font* Window::font = NULL;
		float Window::thickness = 0.04f;

		Window::Window(const std::string &title)
		{
			if (!panelModel)
			{
				panelModel = vrlib::Model::getModel<gl::VertexP3N3T2>("data/vrlib/panel.dae", vrlib::ModelLoadOptions(1.0f));
				shader = new vrlib::gl::ShaderProgram("data/vrlib/GuiShader.vert", "data/vrlib/GuiShader.frag");
				shader->bindAttributeLocation("a_position", 0);
				shader->bindAttributeLocation("a_normal", 1);
				shader->bindAttributeLocation("a_texture", 2);
				shader->link();

				font = new Font("Tahoma");

			}
			rootPanel = NULL;
		}


		void Window::draw(const glm::mat4& projectionMatrix, const glm::mat4 &viewMatrix)
		{
			shader->use();
			shader->setUniformVec4("colorMult", glm::vec4(1, 1, 1, 1));
			shader->setUniformMatrix4("projectionmatrix", projectionMatrix);
			shader->setUniformMatrix4("cameraMatrix", viewMatrix);

			/*glm::mat4 mat = renderMatrix;
			mat = glm::scale(mat, glm::vec3(size, 1));

			panelModel->draw([&mat](const glm::mat4& matrix)
			{
				shader->setUniformMatrix4("modelMatrix", mat * matrix);
			});*/


			rootPanel->draw(glm::translate(renderMatrix, glm::vec3(0,0, 0)));
		}


		void Window::setSelector(const vrlib::math::Ray& ray)
		{
			this->pointerRay = ray;
			pointerRayInWindowSpace = glm::inverse(renderMatrix) * ray;

			rootPanel->foreach([](components::Component* c) {
				c->hover = false;
			});


		}

		void Window::mouseDown()
		{

		}

		void Window::mouseUp()
		{

		}

		void Window::setRootPanel(components::Panel* panel)
		{
			if (rootPanel)
				delete rootPanel;
			rootPanel = panel;
		}

	}
}