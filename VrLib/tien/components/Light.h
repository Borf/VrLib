#pragma once

#include <glm/glm.hpp>
#include "../Component.h"

namespace vrlib
{
	namespace tien
	{
		namespace components
		{
			class Light : public Component
			{
			public:
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