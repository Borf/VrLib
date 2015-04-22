#include "Model.h"
#include "models/SimpleModel.h"
#include "models/AssimpModel.h"
#include "Log.h"
#include <algorithm>

namespace vrlib
{
	Model* Model::getModel(const std::string &fileName, const ModelLoadOptions &options /*= ModelLoadOptions()*/)
	{
		std::string extension = fileName;
		extension = extension.substr(extension.rfind("."));
		std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

		if (extension == ".shape")
		{
			return new SimpleModel(fileName, options);
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

}