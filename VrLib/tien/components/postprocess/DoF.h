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
				class DoF : public PostProcessor
				{
					enum class Uniforms
					{
						FocalDistance,
						FocalDepth,
						Direction
					};
				public:
					float focalDistance;
					float focalDepth;

					DoF();
					virtual json toJson(json &meshes) const;
					virtual void pass(int index);
				};
			}
		}
	}
}