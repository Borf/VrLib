#pragma once

#include <VrLib/VrLib.h>
#include <glm/glm.hpp>
#
namespace vrlib
{
	class Application
	{
	public:
		glm::vec4 clearColor;

		Application();

		virtual void init() = 0;
		virtual void draw(const glm::mat4 &projectionMatrix, const glm::mat4 &modelViewMatrix) = 0;
		virtual void preFrame(double frameTime, double totalTime) { preFrame(); };
		virtual void preFrame() {};

		virtual void latePreFrame() {};


		virtual void stop();;
	};
}