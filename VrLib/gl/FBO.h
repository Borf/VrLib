#pragma once


#ifdef ANDROID
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#else
#include <GL/glew.h>
#endif

#ifdef WIN32
#include <windows.h>
#endif


namespace vrlib
{
	namespace gl
	{
		class FBO
		{
			int width;
			int height;
			GLuint depthBuffer;
			GLuint depthTexture;
			int textureCount;
		public:
			GLuint fboId;
			GLint oldFBO;
			enum Type
			{
				Color,
				Normal,
				Position,
				Depth,
				None,
			};

			FBO(int width, int height, bool depth = false, int textureCount = 1, bool hasDepthTexture = false);
			FBO(int width, int height, bool hasDepthTexture, Type buf1, Type buf2 = None, Type buf3 = None, Type buf4 = None);

			//~FBO();

			void bind();
			void unbind();


			void use(int offset = 0);

			GLuint texid[5];	//5 should be big enough

			int getHeight();
			int getWidth();

		};
	}
}