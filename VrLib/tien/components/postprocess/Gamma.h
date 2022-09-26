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
				class Gamma : public PostProcessor
				{
					enum class Uniforms
					{
						gamma
					};
				public:
					float gamma = 1.0;
					Gamma();
					virtual nlohmann::json toJson(nlohmann::json &meshes) const;
				};
			}
		}
	}
}