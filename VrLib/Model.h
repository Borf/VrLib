#pragma once

#include <functional>
#include <vector>
#include <glm/glm.hpp>
#include <VrLib/math/aabb.h>

namespace vrlib
{
	class Texture;
	class ModelLoadOptions
	{
	public:
		enum Options
		{
			KeepVerts = 1,
			RepositionToCenter = 2,
			RepositionToCenterBottom = 4,
		};

		ModelLoadOptions() {};
		ModelLoadOptions(float size) { this->size = size; };
		ModelLoadOptions(float size, int options) { this->size = size; this->options = options; };
		ModelLoadOptions(float size, bool keepVerts) { this->size = size; this->options = KeepVerts; };

		int options = 0;

		float size = -1;
	};

	class Model;
	class Material
	{
	public:
		Texture* texture = nullptr;
		Texture* normalmap = nullptr;
		Texture* specularmap = nullptr;
		struct
		{
			glm::vec4 diffuse = glm::vec4(0.7f, 0.7f, 0.7f, 1.0f);
			glm::vec4 ambient = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
			glm::vec3 specular = glm::vec3(1, 1, 1);
			float shinyness = 0;
		} color;

	};

	class ModelInstance
	{
	public:
		Model* model;

		ModelInstance(Model* model);

		
		virtual void update(double elapsedTime) {};
		virtual void draw(const std::function<void(const glm::mat4&)> &modelviewMatrixCallback = nullptr, const std::function<bool(const Material&)> &materialCallback = nullptr);

	};


	class Model
	{
	protected:
		Model(){};
	public:
		math::AABB aabb;

		template<class VertexFormat>
		static Model* getModel(const std::string &fileName, const ModelLoadOptions &options = ModelLoadOptions());


		virtual std::vector<glm::vec3> getVertices(int amount) const = 0;
		virtual std::vector<glm::vec3> getTriangles() const = 0;
		virtual std::pair<std::vector<unsigned int>, std::vector<glm::vec3>> getIndexedTriangles() const { throw "not implemented"; };
		virtual void draw(const std::function<void(const glm::mat4&)> &modelviewMatrixCallback, const std::function<bool(const Material&)> &materialCallback = nullptr) = 0;


		virtual std::vector<Material*> getMaterials() = 0;

		virtual ModelInstance* getInstance() = 0;
		virtual bool hasAlphaMaterial() = 0;

		template<class VertexFormat>
		void handleModelLoadOptions(std::vector<VertexFormat> &vertices, const ModelLoadOptions& options);

		void calculateAABB(const std::vector<glm::vec3> &vertices);

		template<class VertexFormat>
		void scaleToSize(std::vector<VertexFormat> &vertices, float maxSize);

		template<class VertexFormat>
		void recenterToCenter(std::vector<VertexFormat> &vertices);

		template<class VertexFormat>
		void recenterToCenterBottom(std::vector<VertexFormat> &vertices);

		std::vector<glm::vec3> collisions(const vrlib::math::Ray &ray);
		std::vector<float> collisionFractions(const vrlib::math::Ray &ray);


		virtual std::vector<std::string> getAnimationNames() const { return std::vector<std::string>(); };
	};
}