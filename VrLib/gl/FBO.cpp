#include "FBO.h"

namespace vrlib
{
	namespace gl
	{

		FBO::FBO(int width, int height, bool depth /*= false*/, int textureCount)
		{
			oldFBO = 0;
			this->textureCount = textureCount;
			depthBuffer = 0;
			fbo = 0;
			this->width = width;
			this->height = height;

			glGenFramebuffers(1, &fbo);
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);


			glGenTextures(textureCount, texid);
			for (int i = 0; i < textureCount; i++)
			{
				glBindTexture(GL_TEXTURE_2D, texid[i]);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, texid[i], 0);
			}




			if (depth)
			{
				glGenRenderbuffers(1, &depthBuffer);
				glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
#ifdef ANDROID
				glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
#else
				glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
#endif
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
			}
			unbind();
		}

		void FBO::bind()
		{
			glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFBO);
			if (oldFBO < 0)
				oldFBO = 0;
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);
			if (depthBuffer > 0)
				glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
			static GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };
			if (textureCount > 0)
				glDrawBuffers(textureCount, buffers);
		}

		void FBO::unbind()
		{
			glBindFramebuffer(GL_FRAMEBUFFER, oldFBO);
			if (depthBuffer > 0)
				glBindRenderbuffer(GL_RENDERBUFFER, 0);
			static GLenum buffers[] = { GL_BACK };
			glDrawBuffers(1, buffers);
		}

		void FBO::use()
		{
			for (int i = 0; i < textureCount; i++)
			{
				glActiveTexture(GL_TEXTURE0 + i);
				glBindTexture(GL_TEXTURE_2D, texid[i]);
			}
		}

		int FBO::getHeight()
		{
			return height;
		}

		int FBO::getWidth()
		{
			return width;
		}
	}
}
