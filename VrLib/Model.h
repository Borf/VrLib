#pragma once

#include <functional>
#include <vector>
#include <glm/glm.hpp>

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
	class Material
	{
	public:
	};

	class ModelInstance
	{
	public:
		Model* model;

		ModelInstance(Model* model);

		virtual void draw(const std::function<void()> &modelviewMatrixCallback = nullptr, const std::function<void(const Material&)> &materialCallback = nullptr);

	};


	class Model
	{
	protected:
		Model(){};
	public:
		template<class VertexFormat>
		static Model* getModel(const std::string &fileName, const ModelLoadOptions &options = ModelLoadOptions());


		virtual std::vector<glm::vec3> getVertices(int amount) const = 0;
		virtual void draw(const std::function<void()> &modelviewMatrixCallback = nullptr, const std::function<void(const Material&)> &materialCallback = nullptr) = 0;

		virtual ModelInstance* getInstance() = 0;
	};
}