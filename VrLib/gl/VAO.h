#pragma once

#include <GL/glew.h>
#include "VBO.h"

namespace vrlib
{
	namespace gl
	{

		template<class T>
		class VAO
		{
		private:
			GLuint vao;
			VAO(const VAO &other);
		public:
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
