#include "Window.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <VrLib/Model.h>
#include <VrLib/gl/Vertex.h>
#include <VrLib/gl/shader.h>
#include <VrLib/gui/components/Panel.h>
#include <VrLib/Font.h>
#include <VrLib/math/Plane.h>


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

			glm::mat4 mat = renderMatrix;
			mat = glm::scale(mat, glm::vec3(1, -1, 1));

			glCullFace(GL_FRONT);
			rootPanel->draw(glm::translate(mat, glm::vec3(0,0, 0)));
			glCullFace(GL_BACK);


			shader->setUniformVec4("colorMult", glm::vec4(100, 0, 0, 1));
			shader->setUniformMatrix4("modelMatrix", glm::mat4());
			gl::VertexP3N3T2 v;
			std::vector<vrlib::gl::VertexP3N3T2> verts;
			setN3(v, glm::vec3(0, 1, 0));
			setP3(v, pointerRay.mOrigin);							setT2(v, glm::vec2(0.5f, 0));			verts.push_back(v);
			setP3(v, pointerRay.mOrigin + 3.0f * pointerRay.mDir);	setT2(v, glm::vec2(0.5f, 0));			verts.push_back(v);			
			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			gl::setAttributes<gl::VertexP3N3T2>(verts.data());
			glLineWidth(10.0f);
			glDrawArrays(GL_LINES, 0, 2);
			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
			glDisableVertexAttribArray(2);
			shader->setUniformVec4("colorMult", glm::vec4(1, 1, 1, 1));

		}


		void Window::setSelector(const vrlib::math::Ray& ray)
		{
			this->pointerRay = ray;
			pointerRayInWindowSpace = glm::inverse(glm::scale(renderMatrix, glm::vec3(1, -1, 1))) * ray;

			rootPanel->foreachWithMatrix([this](const glm::mat4 &parentMatrix, components::Component* c)
			{
				if (c->getBoundingBox(parentMatrix).hasRayCollision(pointerRayInWindowSpace, 0, 100))
				{
					c->hover = true;

					if (c->mousedown)
					{
						math::AABB bb = c->getBoundingBox(parentMatrix);
						math::Plane plane(glm::vec3(0, 0, -1), bb.bounds[1].z);
						glm::vec3 clickPos = plane.getCollisionPoint(pointerRayInWindowSpace);

						clickPos = glm::vec3(glm::inverse(parentMatrix) * glm::vec4(clickPos,0));

						c->drag(clickPos);
					}
				}
				else
					c->hover = false;
			});

		}

		void Window::mouseDown()
		{
			rootPanel->foreachWithMatrix([this](const glm::mat4 &parentMatrix, components::Component* c)
			{
				if (c->getBoundingBox(parentMatrix).hasRayCollision(pointerRayInWindowSpace, 0, 100))
					c->mousedown = true;
				else
					c->mousedown = false;
			});
		}

		void Window::mouseUp()
		{
			rootPanel->foreachWithMatrix([this](const glm::mat4 &parentMatrix, components::Component* c)
			{
				if (c->getBoundingBox(parentMatrix).hasRayCollision(pointerRayInWindowSpace, 0, 100))
					if (c->mousedown)
						c->click();
				c->mousedown = false;
			});
		}

		void Window::setRootPanel(components::Panel* panel)
		{
			if (rootPanel)
				delete rootPanel;
			rootPanel = panel;
		}

		void Window::setComponent(const std::string &name, components::Component* component)
		{
			if (rootPanel)
				rootPanel->setComponent(name, component);
		}

		void Window::setSize(const glm::vec2 size)
		{
			this->size = size;
		}

	}
}