#pragma once

#include <VrLib/VrLib.h>
#include <glm/glm.hpp>
#include <string>

namespace vrlib
{
	class Application
	{
	public:
		glm::vec4 clearColor;
		float farplane = 200.0f;
		float nearplane = 0.1f;
		std::string title = "";

		Application();

		virtual void init() = 0;
		virtual void draw(const glm::mat4 &projectionMatrix, const glm::mat4 &modelViewMatrix) = 0;
		virtual void preFrame(double frameTime, double totalTime) { preFrame(); };
		virtual void preFrame() {};

		virtual void latePreFrame() {};


		virtual void stop();;
	};
}