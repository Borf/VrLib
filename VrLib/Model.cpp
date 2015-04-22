#include "Model.h"
#include "models/SimpleModel.h"
#include "models/AssimpModel.h"
#include "Log.h"
#include <algorithm>

namespace vrlib
{
	template<class VertexFormat>
	Model* Model::getModel(const std::string &fileName, const ModelLoadOptions &options /*= ModelLoadOptions()*/)
	{
		logger << "Loading " << fileName << Log::newline;
		std::string extension = fileName;
		extension = extension.substr(extension.rfind("."));
		std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

		if (extension == ".shape")
		{
			return new SimpleModel<VertexFormat>(fileName, options);
		}



		logger << "Error loading model " << fileName << ", file extension not known ("<<extension<<")" << Log::newline;
		return NULL;
	}




	ModelInstance::ModelInstance(Model* model)
	{
		this->model = model;
	}
	void ModelInstance::draw(gl::ShaderProgram* shader)
	{
		model->draw(shader);
	}

	template Model* Model::getModel< gl::VertexP3 >(const std::string &fileName, const ModelLoadOptions &options);
	template Model* Model::getModel< gl::VertexP3N3 >(const std::string &fileName, const ModelLoadOptions &options);
	template Model* Model::getModel< gl::VertexP3N3T2 >(const std::string &fileName, const ModelLoadOptions &options);
}