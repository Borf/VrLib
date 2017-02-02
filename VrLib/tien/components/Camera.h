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
				Camera(const json &data);
				~Camera();

				bool useFbo = false;
				glm::ivec2 fboSize;
				vrlib::gl::FBO* target = nullptr;

				void render();
				json toJson(json &meshes) const override;
				virtual void buildEditor(EditorBuilder* builder, bool folded) override;

			};
		}
	}
}