#include "NormalApp.h"
#include <VrLib/Kernel.h>
#include <VrLib/Viewport.h>
#include <windows.h>



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
		kernel->config["local"]["window"]["width"] = 1024;
		kernel->config["local"]["window"]["height"] = 768;
		kernel->config["local"]["window"]["border"] = true;
		kernel->config["local"]["window"]["vsync"] = false;
		kernel->config["local"]["window"]["cursor"] = true;

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
