#pragma once

#include <glm/glm.hpp>

#include "../Component.h"

namespace vrlib
{
	namespace gl { class FBO; }
	namespace tien
	{
		namespace components
		{
			class Camera : public Component
			{
			public:
				Camera();
				~Camera();
				vrlib::gl::FBO* target;

				void render();
				json::Value toJson(json::Value &meshes) const override;
			};
		}
	}
}