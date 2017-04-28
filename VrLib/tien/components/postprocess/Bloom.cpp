#include "Bloom.h"

#include <VrLib/gl/Vertex.h>

vrlib::tien::components::postprocessors::Bloom::Bloom() : PostProcessor("Bloom")
{
	loadShader<Uniforms>("data/vrlib/tien/shaders/postprocess/Bloom.vert", "data/vrlib/tien/shaders/postprocess/Bloom.frag");
	addShaderParameter(Uniforms::Strength, "strength", strength);
	shader->registerUniform((int)Uniforms::Direction, "direction");
	passes = 2;
}


void vrlib::tien::components::postprocessors::Bloom::pass(int index)
{
	shader->setUniform((int)Uniforms::Direction, (index % 2 == 0) ? 1 : 0);
}




json vrlib::tien::components::postprocessors::Bloom::toJson(json & meshes) const
{
	json ret;
	ret["type"] = "postprocess";
	ret["postprocess"] = "bloom";
	return ret;
}
