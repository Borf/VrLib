#pragma once


#include <VrLib/Viewport.h>
#include <glm/glm.hpp>
#include <GL/glew.h>

namespace vr
{
	class IVRSystem;
}

namespace vrlib
{
	class OpenVRDriver;
	namespace gl
	{
		class FBO;
		class ShaderProgram;
	}



	class OpenVRViewport : public Viewport
	{
		int eye;
		OpenVRDriver* openVRDriver;
		vr::IVRSystem* m_pHMD;
		uint32_t m_nRenderWidth, m_nRenderHeight;

		glm::mat4 projectionLeft;
		glm::mat4 projectionRight;

		glm::mat4 viewLeft;
		glm::mat4 viewRight;

		glm::mat4 m_rmat4DevicePose[10];


		struct FramebufferDesc
		{
			GLuint m_nDepthBufferId;
			GLuint m_nRenderTextureId;
			GLuint m_nRenderFramebufferId;
			GLuint m_nResolveTextureId;
			GLuint m_nResolveFramebufferId;
		};
		FramebufferDesc leftEyeDesc;
		FramebufferDesc rightEyeDesc;

	public:
		OpenVRViewport(User* user, OpenVRDriver* openVRDriver, Kernel* kernel);


		virtual void draw(Application* application);
		virtual glm::mat4 getProjectionMatrix();


		bool CreateFrameBuffer(int nWidth, int nHeight, FramebufferDesc &framebufferDesc);
	};
}