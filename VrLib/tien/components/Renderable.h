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
					virtual void useCubemap(bool use) {};
				};
				bool visible = true;
				RenderContext* renderContextDeferred = nullptr;
				RenderContext* renderContextShadow = nullptr;
				RenderContext* renderContextForward = nullptr;
				virtual void drawDeferredPass() = 0;
				virtual void drawForwardPass() = 0;
				virtual void drawShadowMap() = 0;
			};
		}
	}
}
