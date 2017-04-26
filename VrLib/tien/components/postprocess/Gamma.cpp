#include "Gamma.h"

#include <VrLib/gl/Vertex.h>

vrlib::tien::components::postprocessors::Gamma::Gamma()
{
	shader = new vrlib::gl::Shader<Uniforms>("data/vrlib/tien/shaders/postprocess/Gamma.vert", "data/vrlib/tien/shaders/postprocess/Gamma.frag");
	shader->bindAttributeLocation("a_position", 0);
	shader->link();
	shader->bindFragLocation("fragColor", 0);
	shader->registerUniform(Uniforms::gamma, "gamma");
}


void vrlib::tien::components::postprocessors::Gamma::process()
{
	shader->use();
	shader->setUniform(Uniforms::gamma, gamma);

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




void vrlib::tien::components::postprocessors::Gamma::buildEditor(EditorBuilder * builder, bool folded)
{
	builder->addTitle("Gamma");

	builder->beginGroup("Gamma");
	builder->addFloatBox(gamma, 0, 10, [this](float newValue) { gamma = newValue; });
	builder->endGroup();
}

json vrlib::tien::components::postprocessors::Gamma::toJson(json & meshes) const
{
	json ret;
	ret["type"] = "postprocess";
	ret["postprocess"] = "gamma";
	ret["gamma"] = gamma;
	return ret;
}
