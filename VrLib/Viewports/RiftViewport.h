#pragma once


#include <VrLib/Viewport.h>
#include <glm/glm.hpp>


#define OVR_OS_WIN32
#include <gl/glew.h>
#include <OVR_CAPI.h>
#include <OVR_CAPI_GL.h>


namespace vrlib
{
	class OculusDeviceDriver;
	namespace gl
	{
		class FBO;
		class ShaderProgram;
	}

	class RiftViewport : public Viewport
	{
		int eye;
		OculusDeviceDriver* oculusDriver;
		gl::FBO* fbo;
		gl::ShaderProgram* shader;

		ovrGLTexture fb_ovr_tex[2];

	public:
		RiftViewport(User* user, OculusDeviceDriver* oculusDriver, Kernel* kernel);


		virtual void draw(Application* application);
		virtual glm::mat4 getProjectionMatrix();

	};
}