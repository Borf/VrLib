#pragma once

#ifdef WIN32
#include <Windows.h>
#endif
#include <GL/glew.h>


namespace vrlib
{
	namespace gl
	{
		template <class T>
		class VAB
		{
		private:
			GLuint id;

			T* element;
			int length;
			VAB(const id &other)
			{
				throw "do not copy!";
			}

		public:
			VAB()
			{
				id = -1;
				length = 0;
				element = NULL;
				glGenBuffers(1, &id);
			}
			~VAB()
			{
				glDeleteBuffers(1, &id);
			}

			void setData(int length, T* data, GLenum usage)
			{
				this->length = length;
				bind();
				glBufferData(GL_ARRAY_BUFFER, sizeof(T) * length, data, usage);
			}


			void bind()
			{
				glBindBuffer(GL_ARRAY_BUFFER, id);
			}

			void unBind()
			{
				glBindBuffer(GL_ARRAY_BUFFER, 0);
			}

			int elementSize()
			{
				return T::getSize();
			}

			int getLength()
			{
				return length;
			}

			void setAttributeIndex(int index)
			{
				glBindBuffer(GL_ARRAY_BUFFER, id);
				glVertexAttribPointer(index, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glVertexAttribDivisor(index, 1);
			}

			T* mapData(GLenum access)
			{
				bind();
				element = (T*)glMapBuffer(GL_ARRAY_BUFFER, access);
				return element;
			}
			void unmapData()
			{
				bind();
				glUnmapBuffer(GL_ARRAY_BUFFER);
				element = NULL;
			}

			T& operator [](int index)
			{
				if (element == NULL)
					throw "Use mapData before accessing";
				return element[index];
			}
		};
	}
}
