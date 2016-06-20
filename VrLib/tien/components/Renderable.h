#pragma once

#include "../Component.h"
#include <VrLib/Singleton.h>
#include <VrLib/gl/shader.h>

namespace vrlib
{
	namespace tien
	{
		namespace components
		{
			class Renderable : public vrlib::tien::Component
			{
			public:
				class RenderContext
				{
				public:
					virtual void init() = 0;
					virtual void frameSetup(const glm::mat4 &projectionMatrix, const glm::mat4 &viewMatrix) = 0;
				};


				RenderContext* renderContext;
				virtual void draw() = 0;
			};
		}
	}
}
