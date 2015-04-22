#pragma once

#include <VrLib/gl/shader.h>

namespace vrlib
{
	class ModelLoadOptions
	{
	public:
		ModelLoadOptions() {};
		ModelLoadOptions(float size) {};
		ModelLoadOptions(float size, bool keepVerts) {};
	};

	class Model;

	class ModelInstance
	{
	public:
		Model* model;

		ModelInstance(Model* model);

		virtual void draw(gl::ShaderProgram* shader);;

	};


	class Model
	{
	protected:
		Model(){};
	public:
		static Model* getModel(const std::string &fileName, const ModelLoadOptions &options = ModelLoadOptions());


		virtual std::vector<glm::vec3> getVertices(int amount) = 0;
		virtual void draw(gl::ShaderProgram* shader) = 0;

		virtual ModelInstance* getInstance() = 0;
	};
}