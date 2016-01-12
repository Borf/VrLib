#pragma once
#include <vrlib/gl/VBO.h>
#include <vrlib/gl/Vertex.h>
#include <vrlib/gl/shader.h>
#include <glm\glm.hpp>

#include <string>
#include <vector>

#include <GL\glew.h>
#include <gl\GL.h>


namespace vrlib
{
	namespace gl
	{
		class ShaderProgram;

		class Cubemap
		{
			ShaderProgram* shader;
			vrlib::gl::VBO<vrlib::gl::VertexPosition>* vbo;

			GLuint texid;

			int width;
			int height;
			int components;

		public:
			vrlib::gl::Cubemap(vrlib::gl::ShaderProgram* shader, std::string filenames[6], const std::vector<glm::vec3> &cubemapVertices);
			~Cubemap(void);

			void bindTexture();
			void draw(const glm::mat4 &projectionMatrix, const glm::mat4 &viewMatrix);
		};

	}
}
