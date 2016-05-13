#define NOMINMAX

#include "OpenVRViewport.h"

#include <VrLib/Application.h>
#include <VrLib/User.h>
#include <VrLib/drivers/Oculus.h>
#include <VrLib/Kernel.h>

#include <VrLib/gl/FBO.h>
#include <VrLib/gl/Shader.h>
#include <VrLib/Log.h>

#include <gl/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GL/wglew.h>
#include <OVR_CAPI_GL.h>
#include <Extras/OVR_Math.h>
namespace vrlib
{

	OpenVRViewport::OpenVRViewport(User* user, OpenVRDriver* oculusDriver, Kernel* kernel) : Viewport(user)
	{


	}

	glm::mat4 OpenVRViewport::getProjectionMatrix()
	{
		return glm::perspective(glm::radians(60.0f), 1.0f, 0.001f, 1000.0f);
	}


	void OpenVRViewport::draw(Application* application)
	{

	}
}