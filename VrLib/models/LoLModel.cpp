#include "LoLModel.h"
#include <VrLib/Log.h>
#include <VrLib/util.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include <algorithm>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>



namespace vrlib
{

	std::string replace(std::string str, std::string toReplace, std::string replacement)
	{
		size_t index = 0;
		while (true)
		{
			index = str.find(toReplace, index);
			if (index == std::string::npos)
				break;
			str.replace(index, toReplace.length(), replacement);
			++index;
		}
		return str;
	}

	std::vector<std::string> split(std::string str, std::string sep)
	{
		std::vector<std::string> ret;
		size_t index;
		while (true)
		{
			index = str.find(sep);
			if (index == std::string::npos)
				break;
			ret.push_back(str.substr(0, index));
			str = str.substr(index + 1);
		}
		ret.push_back(str);
		return ret;
	}

	inline std::string toLower(std::string data)
	{
		std::transform(data.begin(), data.end(), data.begin(), ::tolower);
		return data;
	}


	struct SkinModelHeader {
		//Structure for the Header in skn files
		int magic;
		short numObjects;
		short numMaterials;
	};
	struct SkinModelMaterial {
		//Structure for a material block in skn files
		char name[64];
		int startVertex;
		int numVertices;
		int startIndex;
		int numIndices;
	};

	struct SkinModelVertex {
		//Vertex block in skn files
		float position[3];
		char  boneIndex[4];
		float weights[4];
		float normal[3];
		float texcoords[2];
	};

	struct SkinModelData {
		//data block in skn files
		int numIndices;
		int numVertices;
		std::vector<short> indices;
		std::vector<SkinModelVertex> vertices;
	};






	struct SkinSkelHeader {
		//Structure for header on skl files
		char fileType[8];
		int numObjects;
		int skeletonHash;
		int numElements;
	};

	struct SkinSkelBone {
		//Structure for a bone in skl files
		char name[32];
		int parent;
		float scale;
		float matrix[12];
	};


	struct AnimationHeader {
		char filetype[8];
		int three;
		int magic;
		int numBones;
		int numFrames;
		int fps;
	};

	struct AnimationBone {
		char boneName[32];
		int flag;
	};

	struct AnimationFrame {
		float quat[4];
		float position[3];
	};


	template<class VertexFormat>
	LoLModel<VertexFormat>::LoLModel(const std::string &fileName, const ModelLoadOptions &options /*= ModelLoadOptions()*/)
	{
		std::vector<VertexFormat> verts;

		std::ifstream pFile(fileName.c_str(), std::ios_base::binary);
		if (pFile.bad() || !pFile.good() || !pFile.is_open())
		{
			throw(("Could not open " + fileName).c_str());
			return;
		}


		SkinModelHeader header;
		pFile.read((char*)&header, sizeof(SkinModelHeader));

		if (header.numMaterials == 1)
		{
			int nMaterials;
			pFile.read((char*)&nMaterials, 4);
			header.numMaterials = nMaterials;
		}


		std::vector<SkinModelMaterial> materials;
		materials.resize(header.numMaterials);
		for (int i = 0; i < header.numMaterials; i++)
		{
			pFile.read((char*)&materials[i], sizeof(SkinModelMaterial));
		}


		int nIndices;
		int nVertices;

		pFile.read((char*)&nIndices, 4);
		pFile.read((char*)&nVertices, 4);

		if (nIndices < 0 || nVertices < 0 || nIndices > 200000000 || nVertices > 200000000)
		{
			printf("unable to open %s\n", fileName.c_str());
			throw "oops";
		}

		short* _indices = new short[nIndices];
		pFile.read((char*)_indices, nIndices * 2);

		SkinModelVertex* _vertices = new SkinModelVertex[nVertices];
		pFile.read((char*)_vertices, nVertices*sizeof(SkinModelVertex));


		for (int i = 0; i < nIndices; i++)
			indices.push_back(_indices[i]);

		for (int i = 0; i < nVertices; i++)
		{
			VertexFormat v;

			setP3(v, glm::vec3(_vertices[i].position[0], _vertices[i].position[1], _vertices[i].position[2]));
			setN3(v, glm::vec3(_vertices[i].normal[0], _vertices[i].normal[1], _vertices[i].normal[2]));
			setT2(v, glm::vec2(_vertices[i].texcoords[0], _vertices[i].texcoords[1]));

			verts.push_back(v);
		}




		if (!verts.empty())
		{
			handleModelLoadOptions(verts, options);


			for (size_t i = 0; i < verts.size(); i++)
				vertices.push_back(glm::vec3(verts[i].px, verts[i].py, verts[i].pz));

			vbo.bind();
			vbo.setData(verts.size(), &verts[0], GL_STATIC_DRAW);

			vao = NULL;
			if (!indices.empty())
			{
				vao = new gl::VAO<VertexFormat>(&vbo);
				vio.bind();
				vio.setData(indices.size(), &indices[0], GL_STATIC_DRAW);
				vao->unBind();
			}
		}
		else
			vao = NULL;

	}



	template<class VertexFormat>
	std::vector<glm::vec3> LoLModel<VertexFormat>::getVertices(int amount) const
	{
		return vertices;
	}

	template<class VertexFormat>
	std::vector<glm::vec3> LoLModel<VertexFormat>::getTriangles() const
	{
		return vertices;
	}


	template<class VertexFormat>
	void LoLModel<VertexFormat>::draw(const std::function<void(const glm::mat4&)> &modelviewMatrixCallback, const std::function<void(const Material&)> &materialCallback)
	{
		if (vao)
		{
			vao->bind();
			if (modelviewMatrixCallback)
				modelviewMatrixCallback(glm::mat4());
			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, 0);
			vao->unBind();
		}
	

	}

	template<class VertexFormat>
	ModelInstance* LoLModel<VertexFormat>::getInstance()
	{
		return new ModelInstance(this);
	}


	template class LoLModel < gl::VertexP3 >;
	template class LoLModel < gl::VertexP3N3 >;
	template class LoLModel < gl::VertexP3N3T2 >;
	template class LoLModel < gl::VertexP3N3T2B4B4 >;
	template class LoLModel < gl::VertexP3N2B2T2T2 >;

}