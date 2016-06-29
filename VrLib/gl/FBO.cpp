#include "FBO.h"
#include <VrLib/Log.h>
#include <glm/glm.hpp>
using vrlib::Log;

namespace vrlib
{
	namespace gl
	{

		FBO::FBO(int width, int height, bool depth /*= false*/, int textureCount, bool hasDepthTexture)
		{
			this->depthTexture = 0;
			oldFBO = -1;
			this->textureCount = textureCount;
			depthBuffer = 0;
			fboId = 0;
			this->width = width;
			this->height = height;

			glGenFramebuffers(1, &fboId);
			glBindFramebuffer(GL_FRAMEBUFFER, fboId);


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

				depthTexture = texid[textureCount];
			}
			unbind();
			glBindTexture(GL_TEXTURE_2D, 0);
		}


		FBO::FBO(int width, int height, bool hasDepthTexture, Type buf1, Type buf2, Type buf3, Type buf4)
		{
			this->depthTexture = 0;
			oldFBO = -1;
			this->textureCount = textureCount;
			depthBuffer = 0;
			fboId = 0;
			this->width = width;
			this->height = height;

			glGenFramebuffers(1, &fboId);
			glBindFramebuffer(GL_FRAMEBUFFER, fboId);

			Type textures[] = { buf1, buf2, buf3, buf4 };
			textureCount = 0;
			for (int i = 0; i < 4; i++)
			{
				if (textures[i] == None)
					continue;
				types[i] = textures[i];
				if (textures[i] == Type::ShadowCube)
				{//http://ogldev.atspace.co.uk/www/tutorial43/tutorial43.html
					glGenTextures(1, &texid[i]);
					glBindTexture(GL_TEXTURE_CUBE_MAP, texid[i]);
					glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
					for (int i = 0; i < 6; i++)
						glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT, 0);
					glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
					textureCount++;
				}
				else
				{
					glGenTextures(1, &texid[i]);
					glBindTexture(GL_TEXTURE_2D, texid[i]);
					if (textures[i] == Color)
						glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
					else if (textures[i] == Normal)
						glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB10_A2, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
					else if (textures[i] == Position)
						glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
					else if (textures[i] == Depth)
						glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);

					textureCount++;
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, texid[i], 0);
				}
			}


			glGenRenderbuffers(1, &depthBuffer);
			glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
#ifdef ANDROID
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
#else
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, width, height);
#endif
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

			if (hasDepthTexture)
			{
				glGenTextures(1, &texid[textureCount]);
				depthTexture = texid[textureCount];
				glBindTexture(GL_TEXTURE_2D, texid[textureCount]);
//				glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

//TODO: either pick one of these depending on if the hasDepthTexture is for a shadow or depth
		//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
				glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);

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
			glBindFramebuffer(GL_FRAMEBUFFER, fboId);
			if (depthBuffer > 0)
				glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
			static GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
			if (textureCount > 0)
				glDrawBuffers(textureCount, buffers);
		}

		void FBO::bind(int index)
		{
			if (oldFBO == -1)
				glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFBO);
			glBindFramebuffer(GL_FRAMEBUFFER, fboId);
			if (depthBuffer > 0)
				glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
			//if (textureCount == 0)
			glDrawBuffer(GL_COLOR_ATTACHMENT0); // No color buffer is drawn to.
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X+index, texid[0], 0); //TODO: texid[0] should be the right texid
		}

		void FBO::unbind()
		{
			glBindFramebuffer(GL_FRAMEBUFFER, glm::max(0, oldFBO));
			if (depthBuffer > 0)
				glBindRenderbuffer(GL_RENDERBUFFER, 0);
			if (oldFBO == 0)
			{
				static GLenum buffers[] = { GL_BACK };
				glDrawBuffers(1, buffers);
			}
			else if(oldFBO != -1)
			{
				static GLenum buffers[] = { GL_COLOR_ATTACHMENT0 };
				glDrawBuffers(1, buffers);
			}
			oldFBO = -1;
		}

		void FBO::use(int offset)
		{
			for (int i = 0; i < textureCount; i++)
			{
				glActiveTexture(GL_TEXTURE0 + i + offset);
				glBindTexture(types[i] == Type::ShadowCube ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D, texid[i]);
			}
			if (depthTexture > 0)
			{
				glActiveTexture(GL_TEXTURE0 + textureCount + offset);
				glBindTexture(GL_TEXTURE_2D, texid[textureCount]);
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
