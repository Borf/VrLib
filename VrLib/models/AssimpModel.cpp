#include "AssimpModel.h"

#include <vrlib/gl/Vertex.h>
#include <vrlib/Texture.h>
#include <vrlib/Log.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace vrlib
{


	template<class VertexFormat>
	void AssimpModel<VertexFormat>::import(const glm::mat4 &matrix, const aiScene* scene, aiNode* node)
	{
		glm::mat4 transform = matrix * glm::transpose(glm::make_mat4((float*)&node->mTransformation));

		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

			std::vector<glm::vec3> faceNormals;
			std::vector<glm::vec3> vertexNormals;
			if (!mesh->HasNormals())
			{
				logger<<"Mesh does not have normals...calculating" << Log::newline;
				for (unsigned int ii = 0; ii < mesh->mNumFaces; ii++)
				{
					const struct aiFace* face = &mesh->mFaces[ii];
					if (face->mNumIndices > 2)
					{
						glm::vec3 v1(mesh->mVertices[face->mIndices[0]].x, mesh->mVertices[face->mIndices[0]].y, mesh->mVertices[face->mIndices[0]].z);
						glm::vec3 v2(mesh->mVertices[face->mIndices[1]].x, mesh->mVertices[face->mIndices[1]].y, mesh->mVertices[face->mIndices[1]].z);
						glm::vec3 v3(mesh->mVertices[face->mIndices[2]].x, mesh->mVertices[face->mIndices[2]].y, mesh->mVertices[face->mIndices[2]].z);
						v1 = glm::vec3(glm::vec4(v1, 1) * matrix);
						v2 = glm::vec3(glm::vec4(v2, 1) * matrix);
						v3 = glm::vec3(glm::vec4(v3, 1) * matrix);

						glm::vec3 normal = glm::normalize(glm::cross(v2 - v1, v3 - v1));
						faceNormals.push_back(normal);
					}
					else
						faceNormals.push_back(glm::vec3(0, 0, 0));
				}


				for (unsigned int i = 0; i < mesh->mNumVertices; i++)
				{
					glm::vec3 normal(0, 0, 0);
					for (unsigned int ii = 0; ii < mesh->mNumFaces; ii++)
					{
						const struct aiFace* face = &mesh->mFaces[ii];
						for (unsigned int iii = 0; iii < face->mNumIndices; iii++)
						{
							if (face->mIndices[iii] == i)
								normal += faceNormals[ii];
						}
					}
					if (glm::length(normal) > 0.1)
						normal = glm::normalize(normal);
					vertexNormals.push_back(normal);
				}
			}



			int vertexStart = vertices.size();
			for (unsigned int ii = 0; ii < mesh->mNumVertices; ii++)
			{
				VertexFormat v;
				
				setP3(v, glm::vec3(mesh->mVertices[ii].x, mesh->mVertices[ii].y, mesh->mVertices[ii].z));
//				vertex = vertex * matrix;

				if (mesh->HasTextureCoords(0))
					setT2(v, glm::vec2(mesh->mTextureCoords[0][ii].x, 1 - mesh->mTextureCoords[0][ii].y));

				glm::vec4 normal;
				if (mesh->HasNormals())
					setN3(v, glm::vec3(mesh->mNormals[ii].x, mesh->mNormals[ii].y, mesh->mNormals[ii].z)); //matrix?
				else
					setN3(v, vertexNormals[i]); //matrix?

				/*		//boneIDs
				data["vertices"].push_back(-1);
				data["vertices"].push_back(-1);
				data["vertices"].push_back(-1);
				data["vertices"].push_back(-1);
				//weights
				data["vertices"].push_back(0.0f);
				data["vertices"].push_back(0.0f);
				data["vertices"].push_back(0.0f);
				data["vertices"].push_back(0.0f);*/

				vertices.push_back(v);
			}
			
			Mesh m;
			m.indexStart = indices.size();
			for (unsigned int ii = 0; ii < mesh->mNumFaces; ii++)
			{
				const struct aiFace* face = &mesh->mFaces[ii];
				assert(face->mNumIndices == 3);

				for (int iii = 0; iii < 3; iii++)
					indices.push_back(vertexStart + (int)face->mIndices[iii]);
			}
			m.indexCount = indices.size() - m.indexStart;

			meshes.push_back(m);
		}
	}



	template<class VertexFormat>
	AssimpModel<VertexFormat>::AssimpModel(const std::string &fileName, const ModelLoadOptions& options)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(fileName, aiProcessPreset_TargetRealtime_Quality | aiProcess_OptimizeMeshes | aiProcess_RemoveRedundantMaterials | aiProcess_OptimizeGraph);
		if (!scene)
		{
			logger<<"Error loading file : " << importer.GetErrorString() << Log::newline;
			return;
		}

		import(glm::mat4(), scene, scene->mRootNode);


		handleModelLoadOptions(vertices, options);
	}



	template<class VertexFormat>
	std::vector<glm::vec3> AssimpModel<VertexFormat>::getVertices(int amount) const
	{
		std::vector<glm::vec3> ret;
		for (const VertexFormat& v : vertices)
			ret.push_back(glm::vec3(v.px, v.py, v.pz));
		
		auto last = std::unique(ret.begin(), ret.end(), [](const glm::vec3 &a, const glm::vec3 &b) { return glm::distance(a, b) < 0.075f;  });
		ret = std::vector<glm::vec3>(ret.begin(), last);

		return ret;
	}

	template<class VertexFormat>
	void AssimpModel<VertexFormat>::draw(const std::function<void()> &modelviewMatrixCallback, const std::function<void(const vrlib::Material&)> &materialCallback)
	{

	}

	template<class VertexFormat>
	ModelInstance* AssimpModel<VertexFormat>::getInstance()
	{
		return NULL;
	}




	template class AssimpModel < gl::VertexP3 > ;
	template class AssimpModel < gl::VertexP3N3 >;
	template class AssimpModel < gl::VertexP3N3T2 >;

}