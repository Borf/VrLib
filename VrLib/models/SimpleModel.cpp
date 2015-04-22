#include "SimpleModel.h"

namespace vrlib
{

	SimpleModel::SimpleModel(const std::string &fileName, const ModelLoadOptions &options /*= ModelLoadOptions()*/)
	{

	}



	std::vector<glm::vec3> SimpleModel::getVertices(int amount)
	{
		return std::vector<glm::vec3>();
	}

	void SimpleModel::draw(gl::ShaderProgram* shader)
	{

	}

	ModelInstance* SimpleModel::getInstance()
	{
		return new ModelInstance(this);
	}

}