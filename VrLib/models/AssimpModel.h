#pragma once

#include <VrLib/gl/VAO.h>
#include <VrLib/gl/VBO.h>
#include <VrLib/gl/VIO.h>
#include <VrLib/Model.h>
#include <functional>

struct aiScene;
struct aiNode;

namespace vrlib
{
	template<class VertexFormat>
	class AssimpModel : public Model
	{
	public:
		class Material : public vrlib::Material
		{
		public:

		};

		class Mesh
		{
		public:
			int indexStart;
			int indexCount;
			glm::mat4 matrix;
			Material material;
		};



		AssimpModel(const std::string &fileName, const ModelLoadOptions& options);

		void import(const glm::mat4 &matrix, const aiScene* scene, aiNode* node);
		virtual std::vector<glm::vec3> getVertices(int amount) const override;
		virtual std::vector<glm::vec3> getTriangles() const override;
		virtual std::pair<std::vector<unsigned int>, std::vector<glm::vec3>> getIndexedTriangles() const override;
		virtual void draw(const std::function<void(const glm::mat4&)> &modelviewMatrixCallback, const std::function<void(const vrlib::Material&)> &materialCallback) override;
		virtual ModelInstance* getInstance() override;


		std::string path;

		std::vector<Mesh> meshes;
		std::vector<VertexFormat> vertices;
		std::vector<unsigned int> indices;

		gl::VBO<VertexFormat> vbo;
		gl::VIO<unsigned int> vio;
		gl::VAO<VertexFormat>* vao;

	};
}