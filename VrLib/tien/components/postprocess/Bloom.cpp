#include "Bloom.h"

#include <VrLib/gl/Vertex.h>

vrlib::tien::components::postprocessors::Bloom::Bloom()
{
	shader = new vrlib::gl::Shader<Uniforms>("data/vrlib/tien/shaders/postprocess/bloom.vert", "data/vrlib/tien/shaders/postprocess/bloom.frag");
	shader->bindAttributeLocation("a_position", 0);
	shader->link();
	shader->bindFragLocation("fragColor", 0);
}


void vrlib::tien::components::postprocessors::Bloom::process()
{
	shader->use();

	std::vector<vrlib::gl::VertexP2> verts;
	vrlib::gl::VertexP2 vert;
	vrlib::gl::setP2(vert, glm::vec2(-1, -1));	verts.push_back(vert);
	vrlib::gl::setP2(vert, glm::vec2(1, -1));	verts.push_back(vert);
	vrlib::gl::setP2(vert, glm::vec2(1, 1));	verts.push_back(vert);
	vrlib::gl::setP2(vert, glm::vec2(-1, 1));	verts.push_back(vert);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	vrlib::gl::setAttributes<vrlib::gl::VertexP2>(&verts[0]);
	glDrawArrays(GL_QUADS, 0, 4);

}




json vrlib::tien::components::postprocessors::Bloom::toJson(json & meshes) const
{
	json ret;
	ret["type"] = "postprocess";
	ret["postprocess"] = "bloom";
	return ret;
}
