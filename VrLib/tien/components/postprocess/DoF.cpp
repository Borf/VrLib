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
					loadShader<Uniforms>("data/vrlib/tien/shaders/postprocess/DoF.vert", "data/vrlib/tien/shaders/postprocess/DoF.frag");
					addShaderParameter(Uniforms::Strength, "strength", strength);
					shader->registerUniform((int)Uniforms::Direction, "direction");
					passes = 2;
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

