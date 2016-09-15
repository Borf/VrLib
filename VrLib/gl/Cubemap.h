#pragma once
#include <VrLib/gl/VBO.h>
#include <VrLib/gl/Vertex.h>
#include <VrLib/gl/shader.h>
#include <glm/glm.hpp>

#include <string>
#include <vector>

#include <GL/glew.h>


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
			Cubemap(ShaderProgram* shader, std::string filenames[6], const std::vector<glm::vec3> &cubemapVertices);
			~Cubemap(void);

			void bindTexture();
			void draw(const glm::mat4 &projectionMatrix, const glm::mat4 &viewMatrix);
		};



		class CubeMap
		{
		public:
			CubeMap();

			GLuint texid;

			void setTexture(int index, const std::string &fileName);
			void bind();
		};
	}
}
