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
		class Material : vrlib::Material
		{
		public:

		};

		class Mesh
		{
		public:
			int indexStart;
			int indexCount;
			glm::mat4 matrix;
		};



		AssimpModel(const std::string &fileName, const ModelLoadOptions& options);

		void import(const glm::mat4 &matrix, const aiScene* scene, aiNode* node);
		virtual std::vector<glm::vec3> getVertices(int amount) const override;
		virtual void draw(const std::function<void()> &modelviewMatrixCallback, const std::function<void(const vrlib::Material&)> &materialCallback) override;
		virtual ModelInstance* getInstance() override;


		std::vector<Mesh> meshes;
		std::vector<VertexFormat> vertices;
		std::vector<unsigned short> indices;

		gl::VBO<VertexFormat> vbo;
		gl::VIO<unsigned short> vio;
		gl::VAO<VertexFormat>* vao;

	};
}