#include "Image.h"
#include "../Window.h"
#include <VrLib/gl/shader.h>
#include <VrLib/gl/Vertex.h>
#include <VrLib/Texture.h>
#include <glm/gtc/matrix_transform.hpp>

namespace vrlib
{
	namespace gui
	{
		namespace components
		{

			void Image::draw(const glm::mat4 &parentMatrix)
			{
				vrlib::gl::ShaderProgram* shader = vrlib::gui::Window::shader;

				shader->setUniformVec4("colorMult", glm::vec4(1.5f, 1.5f, 1.5f, 1));
				shader->setUniformMatrix4("modelMatrix", glm::translate(parentMatrix, glm::vec3(position, 0.05f)));
				gl::VertexP3N3T2 v;
				std::vector<vrlib::gl::VertexP3N3T2> verts;
				setN3(v, glm::vec3(0, 0, 1));

				setP3(v, glm::vec3(0, 0, 0));				setT2(v, glm::vec2(0, 1));			verts.push_back(v);
				setP3(v, glm::vec3(size.x, 0, 0));			setT2(v, glm::vec2(1, 1));			verts.push_back(v);
				setP3(v, glm::vec3(size.x, size.y, 0));		setT2(v, glm::vec2(1, 0));			verts.push_back(v);

				setP3(v, glm::vec3(0, 0, 0));				setT2(v, glm::vec2(0, 1));			verts.push_back(v);
				setP3(v, glm::vec3(size.x, size.y, 0));		setT2(v, glm::vec2(1, 0));			verts.push_back(v);
				setP3(v, glm::vec3(0, size.y, 0));			setT2(v, glm::vec2(0, 0));			verts.push_back(v);
				glBindVertexArray(0);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
				gl::setAttributes<gl::VertexP3N3T2>(verts.data());
				texture->bind();
				glDrawArrays(GL_TRIANGLES, 0, 6);
				glDisableVertexAttribArray(0);
				glDisableVertexAttribArray(1);
				glDisableVertexAttribArray(2);
				shader->setUniformVec4("colorMult", glm::vec4(1, 1, 1, 1));
			}

		}
	}
}