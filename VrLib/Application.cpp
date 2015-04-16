#include "Application.h"
#include <VrLib/Kernel.h>

namespace vrlib
{
	void Application::stop()
	{
		Kernel::getInstance()->stop();

	}

	Application::Application()
	{
		clearColor = glm::vec4(0, 0, 0, 1);
	}

}