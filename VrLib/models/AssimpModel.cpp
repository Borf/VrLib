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
			/*if (!mesh->HasNormals())
			{
				//logger<<"Mesh does not have normals...calculating" << Log::newline;
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
			}*/



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
					//setN3(v, vertexNormals[ii]); //matrix?
					setN3(v, glm::vec3(0, 0, 1));
				vertices.push_back(v);
			}
			
			Mesh m;
			m.indexStart = indices.size();
			for (unsigned int ii = 0; ii < mesh->mNumFaces; ii++)
			{
				const struct aiFace* face = &mesh->mFaces[ii];
				if (face->mNumIndices < 3)
					continue;

				for (int iii = 0; iii < 3; iii++)
				{
					assert(face->mIndices[iii] >= 0 && face->mIndices[iii] < mesh->mNumVertices);
					indices.push_back(vertexStart + (int)face->mIndices[iii]);
				}
			}
			m.indexCount = indices.size() - m.indexStart;
			m.matrix = transform;

			
			aiString texPath;
			
			if (scene->mMaterials[mesh->mMaterialIndex]->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == aiReturn_SUCCESS)
				m.material.texture = vrlib::Texture::loadCached(path + "/" + texPath.C_Str());
			else
				m.material.texture = NULL;
			aiColor4D color;
			if (aiGetMaterialColor(scene->mMaterials[mesh->mMaterialIndex], AI_MATKEY_COLOR_AMBIENT, &color) == aiReturn_SUCCESS)
				m.material.color.ambient = glm::vec4(color.r, color.g, color.b, color.a);
			if (aiGetMaterialColor(scene->mMaterials[mesh->mMaterialIndex], AI_MATKEY_COLOR_DIFFUSE, &color) == aiReturn_SUCCESS)
				m.material.color.diffuse = glm::vec4(color.r, color.g, color.b, color.a);
			if (aiGetMaterialColor(scene->mMaterials[mesh->mMaterialIndex], AI_MATKEY_COLOR_SPECULAR, &color) == aiReturn_SUCCESS)
				m.material.color.specular = glm::vec4(color.r, color.g, color.b, color.a);

			if(m.indexCount > 0)
				meshes.push_back(m);
		}


		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			import(transform, scene, node->mChildren[i]);
		}

	}



	template<class VertexFormat>
	AssimpModel<VertexFormat>::AssimpModel(const std::string &fileName, const ModelLoadOptions& options)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(fileName, aiProcessPreset_TargetRealtime_Quality);
		if (!scene)
		{
			logger<<"Error loading file : " << importer.GetErrorString() << Log::newline;
			return;
		}

		path = "";
		if (fileName.find("/") != std::string::npos)
			path = fileName.substr(0, fileName.rfind("/"));
		
		
		int numVerts = 0;
		int numIndices = 0;
		for (size_t i = 0; i < scene->mNumMeshes; i++)
		{
			numVerts += scene->mMeshes[i]->mNumVertices;
			numIndices += scene->mMeshes[i]->mNumFaces * 3;
		}

		vertices.reserve(numVerts);
		indices.reserve(numIndices);

		import(glm::mat4(), scene, scene->mRootNode);
		handleModelLoadOptions(vertices, options);


		vbo.bind();
		vbo.setData(vertices.size(), &vertices[0], GL_STATIC_DRAW);

		vao = new gl::VAO<VertexFormat>(&vbo);
		vao->bind();
		vio.bind();
		vio.setData(indices.size(), &indices[0], GL_STATIC_DRAW);

		vao->unBind();

//		indices.clear();
//		vertices.clear();
	}



	template<class VertexFormat>
	std::vector<glm::vec3> AssimpModel<VertexFormat>::getVertices(int amount) const
	{
		std::vector<glm::vec3> ret;
		for (const Mesh& mesh : meshes)
		{
			for (int i = mesh.indexStart; i < mesh.indexStart + mesh.indexCount; i++)
			{
				glm::vec3 v = glm::make_vec3(&vertices[indices[i]].px);
				v = glm::vec3(mesh.matrix * glm::vec4(v, 1));
				ret.push_back(glm::vec3(v.x, v.y, v.z));
			}
		}
		
		auto last = std::unique(ret.begin(), ret.end(), [](const glm::vec3 &a, const glm::vec3 &b) { return glm::distance(a, b) < 0.0001f;  });
		ret = std::vector<glm::vec3>(ret.begin(), last);

		return ret;
	}


	template<class VertexFormat>
	std::vector<glm::vec3> AssimpModel<VertexFormat>::getTriangles() const
	{
		std::vector<glm::vec3> ret;
		for (const Mesh& mesh : meshes)
		{
			for (int i = mesh.indexStart; i < mesh.indexStart + mesh.indexCount; i++)
			{
				glm::vec3 v = glm::make_vec3(&vertices[indices[i]].px);
				v = glm::vec3(mesh.matrix * glm::vec4(v, 1));
				ret.push_back(glm::vec3(v.x, v.y, v.z));
			}
		}
		return ret;
	}


	template<class VertexFormat>
	std::pair<std::vector<unsigned short>, std::vector<glm::vec3>> AssimpModel<VertexFormat>::getIndexedTriangles() const
	{
		std::pair<std::vector<unsigned short>, std::vector<glm::vec3>> ret;

//TODO
		return ret;
	}



	template<class VertexFormat>
	void AssimpModel<VertexFormat>::draw(const std::function<void(const glm::mat4&)> &modelviewMatrixCallback, const std::function<void(const vrlib::Material&)> &materialCallback)
	{
		vao->bind();
		for (const Mesh& mesh : meshes)
		{
			if (modelviewMatrixCallback)
				modelviewMatrixCallback(mesh.matrix);
			if (materialCallback)
				materialCallback(mesh.material);
			else
			{
				if (mesh.material.texture)
					mesh.material.texture->bind();
			}
			if (mesh.indexCount == 0)
				continue;
			glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_SHORT, (void*)(mesh.indexStart * sizeof(unsigned short)));
		}
		vao->unBind();
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