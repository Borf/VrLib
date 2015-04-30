#include "Window.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <VrLib/Model.h>
#include <VrLib/gl/Vertex.h>
#include <VrLib/gl/shader.h>


namespace vrlib
{
	class Model;
	namespace gl { class ShaderProgram;  }


	namespace gui
	{
		static vrlib::Model* panelModel = NULL;
		static vrlib::gl::ShaderProgram* shader = NULL;

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
			}





		}


		void Window::draw(const glm::mat4& projectionMatrix, const glm::mat4 &viewMatrix)
		{
			shader->use();
			shader->setUniformMatrix4("projectionmatrix", projectionMatrix);
			shader->setUniformMatrix4("cameraMatrix", viewMatrix);


			glm::mat4 mat = renderMatrix;
			mat = glm::scale(mat, glm::vec3(1.0f, 1.0f, 0.025f));

			shader->setUniformMatrix4("modelMatrix", mat);

			panelModel->draw();

		}


		void Window::setSelector(const vrlib::math::Ray& ray)
		{

		}

		void Window::mouseDown()
		{

		}

		void Window::mouseUp()
		{

		}

	}
}