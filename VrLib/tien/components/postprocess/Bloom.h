#pragma once

#include "../PostProcessor.h"
#include <VrLib/gl/shader.h>

namespace vrlib
{
	namespace tien
	{
		namespace components
		{
			namespace postprocessors
			{
				class Bloom : public PostProcessor
				{
					enum class Uniforms
					{
						Direction,
						Strength
					};
				public:
					float strength;

					Bloom();
					virtual nlohmann::json toJson(nlohmann::json &meshes) const;
					virtual void pass(int index);
				};
			}
		}
	}
}