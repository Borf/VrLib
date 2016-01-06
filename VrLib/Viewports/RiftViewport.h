#pragma once


#include <VrLib/Viewport.h>
#include <glm/glm.hpp>


#define OVR_OS_WIN32
#include <gl/glew.h>
#include <OVR_CAPI_GL.h>

namespace vrlib
{
	class OculusDeviceDriver;
	namespace gl
	{
		class FBO;
		class ShaderProgram;
	}


	//---------------------------------------------------------------------------------------
	struct DepthBuffer
	{
		GLuint        texId;

		DepthBuffer(ovrSizei size, int sampleCount)
		{
			assert(sampleCount <= 1); // The code doesn't currently handle MSAA textures.

			glGenTextures(1, &texId);
			glBindTexture(GL_TEXTURE_2D, texId);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			GLenum internalFormat = GL_DEPTH_COMPONENT24;
			GLenum type = GL_UNSIGNED_INT;
/*			if (GLE_ARB_depth_buffer_float)
			{
				internalFormat = GL_DEPTH_COMPONENT32F;
				type = GL_FLOAT;
			}*/

			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, size.w, size.h, 0, GL_DEPTH_COMPONENT, type, NULL);
		}
		~DepthBuffer()
		{
			if (texId)
			{
				glDeleteTextures(1, &texId);
				texId = 0;
			}
		}
	};

	//--------------------------------------------------------------------------
	struct TextureBuffer
	{
		ovrHmd              hmd;
		ovrSwapTextureSet*  TextureSet;
		GLuint              texId;
		GLuint              fboId;
		ovrSizei          texSize;

		TextureBuffer(ovrHmd hmd, bool rendertarget, bool displayableOnHmd, ovrSizei size, int mipLevels, unsigned char * data, int sampleCount) :
			hmd(hmd),
			TextureSet(nullptr),
			texId(0),
			fboId(0),
			texSize()
		{
			assert(sampleCount <= 1); // The code doesn't currently handle MSAA textures.

			texSize = size;

			if (displayableOnHmd)
			{
				// This texture isn't necessarily going to be a rendertarget, but it usually is.
				assert(hmd); // No HMD? A little odd.
				assert(sampleCount == 1); // ovr_CreateSwapTextureSetD3D11 doesn't support MSAA.

				ovrResult result = ovr_CreateSwapTextureSetGL(hmd, GL_SRGB8_ALPHA8, size.w, size.h, &TextureSet);

				if (OVR_SUCCESS(result))
				{
					for (int i = 0; i < TextureSet->TextureCount; ++i)
					{
						ovrGLTexture* tex = (ovrGLTexture*)&TextureSet->Textures[i];
						glBindTexture(GL_TEXTURE_2D, tex->OGL.TexId);

						if (rendertarget)
						{
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
						}
						else
						{
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
						}
					}
				}
			}
			else
			{
				glGenTextures(1, &texId);
				glBindTexture(GL_TEXTURE_2D, texId);

				if (rendertarget)
				{
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				}
				else
				{
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				}

				glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, texSize.w, texSize.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			}

			if (mipLevels > 1)
			{
				glGenerateMipmap(GL_TEXTURE_2D);
			}

			glGenFramebuffers(1, &fboId);
		}

		~TextureBuffer()
		{
			if (TextureSet)
			{
				ovr_DestroySwapTextureSet(hmd, TextureSet);
				TextureSet = nullptr;
			}
			if (texId)
			{
				glDeleteTextures(1, &texId);
				texId = 0;
			}
			if (fboId)
			{
				glDeleteFramebuffers(1, &fboId);
				fboId = 0;
			}
		}

		ovrSizei GetSize() const
		{
			return texSize;
		}

		void SetAndClearRenderSurface(DepthBuffer* dbuffer)
		{
			auto tex = reinterpret_cast<ovrGLTexture*>(&TextureSet->Textures[TextureSet->CurrentIndex]);

			glBindFramebuffer(GL_FRAMEBUFFER, fboId);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex->OGL.TexId, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, dbuffer->texId, 0);

			glViewport(0, 0, texSize.w, texSize.h);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_FRAMEBUFFER_SRGB);
		}

		void UnsetRenderSurface()
		{
			glBindFramebuffer(GL_FRAMEBUFFER, fboId);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
		}
	};



	class RiftViewport : public Viewport
	{
		int eye;
		OculusDeviceDriver* oculusDriver;


		TextureBuffer * eyeRenderTexture[2] = { nullptr, nullptr };
		DepthBuffer   * eyeDepthBuffer[2] = { nullptr, nullptr };
		ovrHmdDesc hmdDesc;
		ovrGLTexture  * mirrorTexture;
		GLuint          mirrorFBO;
		ovrEyeRenderDesc EyeRenderDesc[2];
	public:
		RiftViewport(User* user, OculusDeviceDriver* oculusDriver, Kernel* kernel);


		virtual void draw(Application* application);
		virtual glm::mat4 getProjectionMatrix();

	};
}