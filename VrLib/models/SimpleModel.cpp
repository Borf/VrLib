#include "SimpleModel.h"
#include <VrLib/Log.h>
#include <VrLib/util.h>

namespace vrlib
{
	template<class VertexFormat>
	SimpleModel<VertexFormat>::SimpleModel(const std::string &fileName, const ModelLoadOptions &options /*= ModelLoadOptions()*/)
	{
		std::vector<std::string> parameters = vrlib::util::split(fileName, ".");
		std::vector<VertexFormat> verts;

		if (parameters[0] == "cavewall")
		{
			VertexFormat v;
			setN3(v, glm::vec3(0, -1, 0));

			setP3(v, glm::vec3(-1.5f, -1.5f, -1.5f));	setT2(v, glm::vec2(0, 0));			verts.push_back(v);
			setP3(v, glm::vec3(1.5f, -1.5f, -1.5f));	setT2(v, glm::vec2(0.5f, 0));		verts.push_back(v);
			setP3(v, glm::vec3(1.5f, -1.5f, 1.5f));		setT2(v, glm::vec2(0.5f, 0.5f));	verts.push_back(v);
			setP3(v, glm::vec3(-1.5f, -1.5f, 1.5f));	setT2(v, glm::vec2(0, 0.5f));		verts.push_back(v);

			setN3(v, glm::vec3(1, 0, 0));
			setP3(v, glm::vec3(1.5f, -1.5f, -1.5f));	setT2(v, glm::vec2(0.5f, 0));		verts.push_back(v);
			setP3(v, glm::vec3(1.5f, 1.5f, -1.5f));		setT2(v, glm::vec2(0.5f, 0.5f));	verts.push_back(v);
			setP3(v, glm::vec3(1.5f, 1.5f, 1.5f));		setT2(v, glm::vec2(1.0f, 0.5f));	verts.push_back(v);
			setP3(v, glm::vec3(1.5f, -1.5f, 1.5f));		setT2(v, glm::vec2(1.0f, 0));		verts.push_back(v);

			setN3(v, glm::vec3(-1, 0, 0));
			setP3(v, glm::vec3(-1.5f, -1.5f, 1.5f));	setT2(v, glm::vec2(1.0f, 0));		verts.push_back(v);
			setP3(v, glm::vec3(-1.5f, 1.5f, 1.5f));		setT2(v, glm::vec2(1.0f, 0.5f));	verts.push_back(v);
			setP3(v, glm::vec3(-1.5f, 1.5f, -1.5f));	setT2(v, glm::vec2(0.5f, 0.5f));	verts.push_back(v);
			setP3(v, glm::vec3(-1.5f, -1.5f, -1.5f));	setT2(v, glm::vec2(0.5f, 0));		verts.push_back(v);

			setN3(v, glm::vec3(0, 0, -1));
			setP3(v, glm::vec3(-1.5f, -1.5f, -1.5f));	setT2(v, glm::vec2(0.5f, 0));		verts.push_back(v);
			setP3(v, glm::vec3(-1.5f, 1.5f, -1.5f));	setT2(v, glm::vec2(0.5f, 0.5f));	verts.push_back(v);
			setP3(v, glm::vec3(1.5f, 1.5f, -1.5f));		setT2(v, glm::vec2(1.0f, 0.5f));	verts.push_back(v);
			setP3(v, glm::vec3(1.5f, -1.5f, -1.5f));	setT2(v, glm::vec2(1.0f, 0));		verts.push_back(v);

			indices.clear();
			for (size_t i = 0; i < verts.size(); i += 4)
			{
				indices.push_back(i + 2);
				indices.push_back(i + 1);
				indices.push_back(i + 0);

				indices.push_back(i + 3);
				indices.push_back(i + 2);
				indices.push_back(i + 0);
			}
		}
		else
			logger << "Unknown simplemodel: " << fileName << Log::newline;



		if (!verts.empty())
		{
			vao = new gl::VAO<VertexFormat>(&vbo);

			vbo.bind();
			vbo.setData(verts.size(), &verts[0], GL_STATIC_DRAW);
			vio.bind();
			vio.setData(indices.size(), &indices[0], GL_STATIC_DRAW);

			vao->unBind();
		}
		else
			vao = NULL;

	}



	template<class VertexFormat>
	std::vector<glm::vec3> SimpleModel<VertexFormat>::getVertices(int amount)
	{
		return std::vector<glm::vec3>();
	}

	template<class VertexFormat>
	void SimpleModel<VertexFormat>::draw(gl::ShaderProgram* shader)
	{
		if (vao)
		{
			vao->bind();
			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, 0);
			vao->unBind();
		}
	

	}

	template<class VertexFormat>
	ModelInstance* SimpleModel<VertexFormat>::getInstance()
	{
		return new ModelInstance(this);
	}


	template class SimpleModel < gl::VertexP3 >;
	template class SimpleModel < gl::VertexP3N3 >;
	template class SimpleModel < gl::VertexP3N3T2 > ;

}