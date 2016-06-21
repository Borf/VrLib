#pragma once

#include <VrLib/Application.h>
#include <VrLib/drivers/Keyboard.h>
#include <VrLib/drivers/MouseButton.h>
#include <string>

namespace vrlib
{
	class NormalApp : public vrlib::Application, public vrlib::KeyboardDeviceDriver, public vrlib::MouseButtonDeviceDriver
	{
	public:
		NormalApp(const std::string &title);
		~NormalApp();

		virtual void draw(const glm::mat4 &projectionMatrix, const glm::mat4 &modelViewMatrix) override;
		virtual void draw() = 0;
	};

}