#include "NormalApp.h"
#include <VrLib/Kernel.h>
#include <VrLib/Viewport.h>

#ifdef WIN32
#include <windows.h>
#endif


namespace vrlib
{
	class NullViewPort : public Viewport
	{
	public:
		NullViewPort() : Viewport(NULL) { size = glm::vec2(1, 1);  }
		virtual void draw(Application* application) override {
			if (Kernel::getInstance()->getWindowHeight() == 0 || Kernel::getInstance()->getWindowWidth() == 0)
				return;
			application->draw(glm::mat4(), glm::mat4());
		}
		virtual glm::mat4 getProjectionMatrix() override { return glm::mat4(); }
	};



	NormalApp::NormalApp(const std::string &title)
	{
		Kernel* kernel = Kernel::getInstance();
		kernel->config["local"]["window"]["title"] = title;
		kernel->config["local"]["window"]["x"] = 0;
		kernel->config["local"]["window"]["y"] = 0;
		kernel->config["local"]["window"]["width"] = 1920;
		kernel->config["local"]["window"]["height"] = 1080;
		kernel->config["local"]["window"]["border"] = true;
		kernel->config["local"]["window"]["vsync"] = false;
		kernel->config["local"]["window"]["cursor"] = true;

		char hostname[255];
		gethostname(hostname, 255);
		if(strcmp(hostname, "BorfDesktop") == 0)
			kernel->config["local"]["window"]["vsync"] = true;


		kernel->viewports.push_back(new NullViewPort());

		kernel->keyboardDriver = this;
		kernel->mouseDriver = this;

	}


	NormalApp::~NormalApp()
	{
	}

	void NormalApp::draw(const glm::mat4 &projectionMatrix, const glm::mat4 &modelViewMatrix)
	{
		draw();
	}
}
