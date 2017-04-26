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

					};
					vrlib::gl::Shader<Uniforms>* shader;
				public:
					Bloom();

					virtual json toJson(json &meshes) const;
					virtual void process();
				};
			}
		}
	}
}