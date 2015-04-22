#include "SimpleModel.h"
#include <VrLib/Log.h>
#include <VrLib/util.h>

namespace vrlib
{
	template<class VertexFormat>
	SimpleModel<VertexFormat>::SimpleModel(const std::string &fileName, const ModelLoadOptions &options /*= ModelLoadOptions()*/)
	{
		std::vector<std::string> parameters = vrlib::util::split(fileName, ".");

		if (parameters[0] == "cavewall")
		{

		}
		else
			logger << "Unknown simplemodel: " << fileName << Log::newline;

	}



	template<class VertexFormat>
	std::vector<glm::vec3> SimpleModel<VertexFormat>::getVertices(int amount)
	{
		return std::vector<glm::vec3>();
	}

	template<class VertexFormat>
	void SimpleModel<VertexFormat>::draw(gl::ShaderProgram* shader)
	{

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