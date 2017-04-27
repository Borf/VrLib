#include "PostProcessor.h"
#include <VrLib/gl/Vertex.h>


namespace vrlib
{
	namespace tien
	{
		namespace components
		{
			void vrlib::tien::components::PostProcessor::runPass(int index)
			{
				shader->use();
				//shader->setUniform(Uniforms::gamma, gamma);

				for (const auto parameter : parameters)
				{
					parameter->set(shader);
				}


				pass(index);
			}


			void vrlib::tien::components::PostProcessor::buildEditor(EditorBuilder * builder, bool folded)
			{
				builder->addTitle(name);


				for (const auto parameter : parameters)
				{
					builder->beginGroup(parameter->name);
					parameter->buildEditor(builder);
					builder->endGroup();
				}



			}
		}
	}
}
