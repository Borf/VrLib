#pragma once


#include <VrLib/Viewport.h>
#include <glm/glm.hpp>


#define OVR_OS_WIN32
#include <gl/glew.h>


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

	public:
		OpenVRViewport(User* user, OpenVRDriver* openVRDriver, Kernel* kernel);


		virtual void draw(Application* application);
		virtual glm::mat4 getProjectionMatrix();

	};
}