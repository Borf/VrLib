#include "Gamma.h"

#include <VrLib/gl/Vertex.h>

vrlib::tien::components::postprocessors::Gamma::Gamma() : PostProcessor("Gamma")
{
	loadShader<Uniforms>("data/vrlib/tien/shaders/postprocess/Gamma.vert", "data/vrlib/tien/shaders/postprocess/Gamma.frag");
	addShaderParameter(Uniforms::gamma, "gamma", gamma);
}



json vrlib::tien::components::postprocessors::Gamma::toJson(json & meshes) const
{
	json ret;
	ret["type"] = "postprocess";
	ret["postprocess"] = "gamma";
	ret["gamma"] = gamma;
	return ret;
}
