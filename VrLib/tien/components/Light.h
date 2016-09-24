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
			class Light : public Component
			{
				vrlib::gl::FBO* shadowMapDirectional = nullptr;
				void generateShadowMap();
				glm::mat4 projectionMatrix;
				glm::mat4 modelViewMatrix;

				friend class vrlib::tien::Renderer;
			public:
				Light() {};
				Light(const vrlib::json::Value &json);
				~Light();

				json::Value toJson(json::Value &meshes) const override;


				enum class Type
				{
					directional = 0,
					point = 1,
					spot = 2
				} type = Type::point;

				enum class Baking
				{
					realtime,
					baked
				} baking = Baking::realtime;

				enum class Shadow
				{
					none,
					shadowmap,
					shadowvolume
				} shadow = Shadow::none;



				float intensity;
				glm::vec4 color;


				float spotlightAngle;
				float range; //point / spotlight

			};
		}
	}
}