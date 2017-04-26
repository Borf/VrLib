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
					vrlib::gl::Shader<Uniforms>* shader;
				public:
					float gamma = 1.0;
					Gamma();

					virtual void buildEditor(EditorBuilder* builder, bool folded);
					virtual json toJson(json &meshes) const;
					virtual void process();
				};
			}
		}
	}
}