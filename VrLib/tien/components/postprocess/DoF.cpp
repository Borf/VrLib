#include "DoF.h"

#include <VrLib/gl/Vertex.h>

namespace vrlib
{
	namespace tien
	{
		namespace components
		{
			namespace postprocessors
			{
				DoF::DoF() : PostProcessor("Depth of Field")
				{
					focalDepth = 2;
					focalDistance = 3;

					loadShader<Uniforms>("data/vrlib/tien/shaders/postprocess/DoF.vert", "data/vrlib/tien/shaders/postprocess/DoF.frag");
					addShaderParameter(Uniforms::FocalDistance, "focalDistance", focalDistance);
					addShaderParameter(Uniforms::FocalDepth, "focalDepth", focalDepth);
					shader->registerUniform((int)Uniforms::Direction, "direction");
					passes = 4;
				}


				void DoF::pass(int index)
				{
					shader->setUniform((int)Uniforms::Direction, (index % 2 == 0) ? 1 : 0);
				}

				
				json DoF::toJson(json & meshes) const
				{
					json ret;
					ret["type"] = "postprocess";
					ret["postprocess"] = "dof";
					return ret;
				}

			}
		}
	}
}

