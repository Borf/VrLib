#include "Bloom.h"

#include <VrLib/gl/Vertex.h>

vrlib::tien::components::postprocessors::Bloom::Bloom() : PostProcessor("Bloom")
{
	loadShader<Uniforms>("data/vrlib/tien/shaders/postprocess/Bloom.vert", "data/vrlib/tien/shaders/postprocess/Bloom.frag");
}


void vrlib::tien::components::postprocessors::Bloom::pass(int index)
{

}




json vrlib::tien::components::postprocessors::Bloom::toJson(json & meshes) const
{
	json ret;
	ret["type"] = "postprocess";
	ret["postprocess"] = "bloom";
	return ret;
}
