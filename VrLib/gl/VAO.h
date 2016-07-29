#pragma once

#include <GL/glew.h>
#include "VBO.h"

namespace vrlib
{
	namespace gl
	{

		class VAO
		{
		private:
			GLuint vao;
			VAO(const VAO &other);
		public:
			template<class T>
			VAO(VBO<T>* vbo)
			{
				glGenVertexArrays(1, &vao);
				bind();
				vbo->bind();
				vbo->setAttributes();
				//unBind();
			}

			~VAO()
			{
				glDeleteVertexArrays(1, &vao);
			}

			void bind()
			{
				glBindVertexArray(vao);
			}

			void unBind()
			{
				glBindVertexArray(0);
			}



		};

	}
}
