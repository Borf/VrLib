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
			glm::vec3 dirs[] = { glm::vec3(-1.5f, 0, 0), glm::vec3(1.5f, 0, 0), glm::vec3(0, -1.5f, 0), glm::vec3(0, 0, -1.5f) };


			for (auto normal : dirs)
			{
				glm::vec4 a1(normal, 0);
				glm::vec4 a2(normal, 0);
				for (int i = 2; i >= 0; i--)
					a1[i + 1] = a1[i];
				a1[0] = a1[3];

				for (int ii = 0; ii < 2; ii++)
				{
					for (int i = 2; i >= 0; i--)
						a2[i + 1] = a2[i];
					a2[0] = a2[3];
				}

				VertexFormat v;
				setN3(v, normal);


				setP3(v, normal + glm::vec3(a1 + a2));
				setT2(v, glm::vec2(0, 0));
				verts.push_back(v);

				setP3(v, normal + glm::vec3(a1 - a2));
				setT2(v, glm::vec2(0, 0));
				verts.push_back(v);

				setP3(v, normal - glm::vec3(a1 - a2));
				setT2(v, glm::vec2(0, 0));
				verts.push_back(v);

				setP3(v, normal - glm::vec3(a1 + a2));
				setT2(v, glm::vec2(0, 0));
				verts.push_back(v);
			}

			indices.clear();
			for (size_t i = 0; i < verts.size(); i += 4)
			{
				indices.push_back(i + 0);
				indices.push_back(i + 1);
				indices.push_back(i + 2);

				indices.push_back(i + 1);
				indices.push_back(i + 2);
				indices.push_back(i + 3);
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