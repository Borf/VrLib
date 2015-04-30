#include "SimpleModel.h"
#include <VrLib/Log.h>
#include <VrLib/util.h>

#define _USE_MATH_DEFINES
#include <math.h>


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
			setN3(v, glm::vec3(0, 1, 0));

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

			setN3(v, glm::vec3(0, 0, 1));
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
		if (parameters[0] == "cube")
		{
			VertexFormat v;
			setN3(v, glm::vec3(0, 1, 0));
			setP3(v, glm::vec3(-1.5f, -1.5f, -1.5f));	setT2(v, glm::vec2(0, 0));			verts.push_back(v);
			setP3(v, glm::vec3(1.5f, -1.5f, -1.5f));	setT2(v, glm::vec2(0.5f, 0));		verts.push_back(v);
			setP3(v, glm::vec3(1.5f, -1.5f, 1.5f));		setT2(v, glm::vec2(0.5f, 0.5f));	verts.push_back(v);
			setP3(v, glm::vec3(-1.5f, -1.5f, 1.5f));	setT2(v, glm::vec2(0, 0.5f));		verts.push_back(v);

			setN3(v, glm::vec3(0, -1, 0));
			setP3(v, glm::vec3(-1.5f, 1.5f, 1.5f));		setT2(v, glm::vec2(0, 0.5f));		verts.push_back(v);
			setP3(v, glm::vec3(1.5f, 1.5f, 1.5f));		setT2(v, glm::vec2(0.5f, 0.5f));	verts.push_back(v);
			setP3(v, glm::vec3(1.5f, 1.5f, -1.5f));		setT2(v, glm::vec2(0.5f, 0));		verts.push_back(v);
			setP3(v, glm::vec3(-1.5f, 1.5f, -1.5f));	setT2(v, glm::vec2(0, 0));			verts.push_back(v);

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

			setN3(v, glm::vec3(0, 0, 1));
			setP3(v, glm::vec3(-1.5f, -1.5f, -1.5f));	setT2(v, glm::vec2(0.5f, 0));		verts.push_back(v);
			setP3(v, glm::vec3(-1.5f, 1.5f, -1.5f));	setT2(v, glm::vec2(0.5f, 0.5f));	verts.push_back(v);
			setP3(v, glm::vec3(1.5f, 1.5f, -1.5f));		setT2(v, glm::vec2(1.0f, 0.5f));	verts.push_back(v);
			setP3(v, glm::vec3(1.5f, -1.5f, -1.5f));	setT2(v, glm::vec2(1.0f, 0));		verts.push_back(v);

			setN3(v, glm::vec3(0, 0, -1));
			setP3(v, glm::vec3(1.5f, -1.5f, 1.5f));		setT2(v, glm::vec2(1.0f, 0));		verts.push_back(v);
			setP3(v, glm::vec3(1.5f, 1.5f, 1.5f));		setT2(v, glm::vec2(1.0f, 0.5f));	verts.push_back(v);
			setP3(v, glm::vec3(-1.5f, 1.5f, 1.5f));		setT2(v, glm::vec2(0.5f, 0.5f));	verts.push_back(v);
			setP3(v, glm::vec3(-1.5f, -1.5f, 1.5f));	setT2(v, glm::vec2(0.5f, 0));		verts.push_back(v);

			indices.clear();
			for (size_t i = 0; i < verts.size(); i += 4)
			{
				indices.push_back(i + 0);
				indices.push_back(i + 1);
				indices.push_back(i + 2);

				indices.push_back(i + 0);
				indices.push_back(i + 2);
				indices.push_back(i + 3);
			}
		}
		else if (parameters[0] == "sphere")
		{
			int stacks = 10;
			int slices = 10;

			if (parameters.size() == 4)
			{
				stacks = atoi(parameters[1].c_str());
				slices = atoi(parameters[2].c_str());
			}


			float thetaInc = (float)M_PI / stacks;
			float phiInc = (float)(2 * M_PI / slices);

			for (float theta = -(float)M_PI_2; theta < (float)M_PI_2; theta += thetaInc)
			{
				for (float phi = 0; phi < (float)(2 * M_PI); phi += phiInc)
				{
					float x1 = glm::cos(theta) * glm::cos(phi);
					float y1 = glm::cos(theta) * glm::sin(phi);
					float z1 = glm::sin(theta);

					float x2 = glm::cos(theta + thetaInc) * glm::cos(phi + phiInc);
					float y2 = glm::cos(theta + thetaInc) * glm::sin(phi + phiInc);
					float z2 = glm::sin(theta + thetaInc);

					float x3 = glm::cos(theta + thetaInc) * glm::cos(phi);
					float y3 = glm::cos(theta + thetaInc) * glm::sin(phi);
					float z3 = glm::sin(theta + thetaInc);

					float x4 = glm::cos(theta) * glm::cos(phi + phiInc);
					float y4 = glm::cos(theta) * glm::sin(phi + phiInc);
					float z4 = glm::sin(theta);

					VertexFormat v;
					setP3(v, glm::vec3(x1, y1, z1));		setN3(v, glm::vec3(x1, y1, z1));		setT2(v, glm::vec2(asin(x1)/M_PI + 0.5f, asin(y1)/M_PI + 0.5f));		verts.push_back(v);
					setP3(v, glm::vec3(x2, y2, z2));		setN3(v, glm::vec3(x2, y2, z2));		setT2(v, glm::vec2(asin(x2)/M_PI + 0.5f, asin(y2)/M_PI + 0.5f));		verts.push_back(v);
					setP3(v, glm::vec3(x3, y3, z3));		setN3(v, glm::vec3(x3, y3, z3));		setT2(v, glm::vec2(asin(x3)/M_PI + 0.5f, asin(y3)/M_PI + 0.5f));		verts.push_back(v);
					setP3(v, glm::vec3(x4, y4, z4));		setN3(v, glm::vec3(x4, y4, z4));		setT2(v, glm::vec2(asin(x4)/M_PI + 0.5f, asin(y4)/M_PI + 0.5f));		verts.push_back(v);
				}
			}
			indices.clear();
			for (size_t i = 0; i < verts.size(); i += 4)
			{
				indices.push_back(i + 0);
				indices.push_back(i + 1);
				indices.push_back(i + 2);
				
				indices.push_back(i + 3);
				indices.push_back(i + 1);
				indices.push_back(i + 0);
			}

		}
		else
			logger << "Unknown simplemodel: " << fileName << Log::newline;



		if (!verts.empty())
		{
			handleModelLoadOptions(verts, options);


			for (size_t i = 0; i < verts.size(); i++)
				vertices.push_back(glm::vec3(verts[i].px, verts[i].py, verts[i].pz));


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
	std::vector<glm::vec3> SimpleModel<VertexFormat>::getVertices(int amount) const
	{
		return vertices;
	}

	template<class VertexFormat>
	void SimpleModel<VertexFormat>::draw(const std::function<void()> &modelviewMatrixCallback, const std::function<void(const Material&)> &materialCallback)
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