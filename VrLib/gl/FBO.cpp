#include "FBO.h"
#include <VrLib/Log.h>
using vrlib::Log;

namespace vrlib
{
	namespace gl
	{

		FBO::FBO(int width, int height, bool depth /*= false*/, int textureCount, bool hasDepthTexture)
		{
			this->depthTexture = 0;
			oldFBO = 0;
			this->textureCount = textureCount;
			depthBuffer = 0;
			fbo = 0;
			this->width = width;
			this->height = height;

			glGenFramebuffers(1, &fbo);
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);


			for (int i = 0; i < textureCount; i++)
			{
				glGenTextures(1, &texid[i]);
				glBindTexture(GL_TEXTURE_2D, texid[i]);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


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

			if (hasDepthTexture)
			{
				glGenTextures(1, &texid[textureCount]);
				glBindTexture(GL_TEXTURE_2D, texid[textureCount]);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

				glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texid[textureCount], 0);
				if(textureCount == 0)
					glDrawBuffer(GL_NONE); // No color buffer is drawn to.
			}
			unbind();
			glBindTexture(GL_TEXTURE_2D, 0);
		}


		FBO::FBO(int width, int height, bool hasDepthTexture, Type buf1, Type buf2, Type buf3, Type buf4)
		{
			this->depthTexture = 0;
			oldFBO = 0;
			this->textureCount = textureCount;
			depthBuffer = 0;
			fbo = 0;
			this->width = width;
			this->height = height;

			glGenFramebuffers(1, &fbo);
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);

			Type textures[] = { buf1, buf2, buf3, buf4 };
			textureCount = 0;
			for (int i = 0; i < 4; i++)
			{
				if (textures[i] == None)
					continue;
				glGenTextures(1, &texid[i]);
				glBindTexture(GL_TEXTURE_2D, texid[i]);
				if (textures[i] == Color)
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
				else if (textures[i] == Normal)
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F_ARB, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
				else if (textures[i] == Position)
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
				textureCount++;
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, texid[i], 0);
			}


			glGenRenderbuffers(1, &depthBuffer);
			glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
#ifdef ANDROID
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
#else
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
#endif
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

			if (hasDepthTexture)
			{
				glGenTextures(1, &texid[textureCount]);
				glBindTexture(GL_TEXTURE_2D, texid[textureCount]);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

				glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texid[textureCount], 0);
				if (textureCount == 0)
					glDrawBuffer(GL_NONE); // No color buffer is drawn to.
			}
			unbind();
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		void FBO::bind()
		{
			glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFBO);
			if (oldFBO < 0)
				oldFBO = 0;
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);
			if (depthBuffer > 0)
				glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
			static GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
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
			if(textureCount > 1)
				glActiveTexture(GL_TEXTURE0);

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
