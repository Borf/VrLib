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

#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_inverse.hpp>



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
		short version;
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
		int version;
	};
	
	struct SkinSkelHeader12
	{
		int skeletonHash;
		int numBones;
	};

	struct SkinSkelHeader0
	{
		short zero;
		short numBones;
		int numBoneIds;
		short offsetVertexData;
		short unknown1;
		int offset1;
		int offsetToAnimationIndices;
		int offset2;
		int offset3;
		int offsetToStrings;
		char empty[20];
	};

	struct SkinSkelBone {
		//Structure for a bone in skl files
		char name[32];
		int parent;
		float scale;
		float matrix[12];
	};

	struct SkinSkelBone0 {
		short zero;
		short id;
		short parent;
		short unknown;
		int namehash;
		float twopointone;
		float position[3];
		float scaling[3];
		float orientation[4];
		float ct[3];
		unsigned char padding[32];
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

		if (header.version == 4)
		{
			char buf[60];
			pFile.read(buf, 4);
			pFile.read((char*)&nIndices, 4);
			pFile.read((char*)&nVertices, 4);
			pFile.read(buf, 48);
		}
		else
		{
			pFile.read((char*)&nIndices, 4);
			pFile.read((char*)&nVertices, 4);
		}

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
			for (int i = 0; i < 4; i++)
				gl::setB4(v, i, _vertices[i].boneIndex[i], _vertices[i].weights[i]);

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
				vao = new gl::VAO(&vbo);
				vio.bind();
				vio.setData(indices.size(), &indices[0], GL_STATIC_DRAW);
				vao->unBind();
			}
		}
		else
			vao = NULL;


		{
			std::ifstream pFile((fileName.substr(0, fileName.length() - 4) + ".skl").c_str(), std::ios_base::binary);
			if (!pFile.is_open())
			{
				logger << "Unable to open skeleton file: " << fileName.substr(0, fileName.length() - 4) + ".skl" << Log::newline;
				return;
			}

			SkinSkelHeader header;
			pFile.read((char*)&header, sizeof(SkinSkelHeader));


			if (header.version == 0)
			{
				SkinSkelHeader0 header0;
				pFile.read((char*)&header0, sizeof(SkinSkelHeader0));

				printf("%i\n", header0.numBones);

				pFile.seekg(header0.offsetVertexData, std::ios_base::beg);

				std::vector<SkinSkelBone0> rawbones;
				for (int i = 0; i < header0.numBones; i++)
				{
					SkinSkelBone0 bone;
					pFile.read((char*)&bone, sizeof(SkinSkelBone0));
					rawbones.push_back(bone);
				}

				pFile.seekg(header0.offsetToStrings, std::ios_base::beg);
				std::vector<std::string> names;
				for (int i = 0; i < header0.numBones; i++)
				{
					std::string name;
					while (true)
					{
						char buf[5] = { 0, 0, 0, 0, 0};
						pFile.read(buf, 4);
						if (buf[0] != 0)
							name += buf;
						if (buf[3] == 0)
							break;
					}
					names.push_back(name);
				}

				std::function<void(Bone*)> buildBone;
				buildBone = [&](Bone* bone)
				{
					bone->name = names[bone->id];
					
					SkinSkelBone0& raw = rawbones[bone->id];
					glm::quat rotation(raw.orientation[3], raw.orientation[1], raw.orientation[2], raw.orientation[0]);
					bone->matrix = glm::mat4();

					bone->matrix = bone->matrix * (glm::toMat4(rotation));
					bone->matrix = glm::translate(bone->matrix, glm::vec3(raw.position[0], raw.position[1], raw.position[2]));
					//for (int ii = 0; ii < 3; ii++)
					///	bone->matrix[3][ii] = raw.position[ii];


					for(const auto &b : rawbones)
					{
						if (b.parent == bone->id && b.parent == 0)
						{
							Bone* childbone = new Bone(b.id);
							buildBone(childbone);
							bone->children.push_back(childbone);
							childbone->parent = bone;
						}
					}

				};

				for (size_t i = 0; i < rawbones.size(); i++)
				{
					if (rawbones[i].parent == -1)
					{
						Bone* bone = new Bone(rawbones[i].id);
						buildBone(bone);
						bones.push_back(bone);
					}
				}
				printf("Bones read");

			}

			/*for (int i = 0; i < header.numBones; i++)
			{
				if (header.version == 0)
				{
					SkinSkelBone0 bone;
					pFile.read((char*)&bone, sizeof(SkinSkelBone0));

					printf("%i", bone.id);
				}
				else
				{
					SkinSkelBone bone;
					pFile.read((char*)&bone, sizeof(SkinSkelBone));

					printf("%s", bone.name);
					throw "Not tested";
				}

			}*/




		}

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
		
		glColor4f(1, 1, 1, 1);
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_DEPTH_TEST);
		glUseProgram(0);
		glPushMatrix();
		glScalef(0.01f, 0.01f, 0.01f);
		glLineWidth(1);
		glBegin(GL_LINES);
		
		std::function<void(Bone*, const glm::mat4 &)> drawBone;
		drawBone = [&](Bone* b, const glm::mat4 &parentMat)
		{
			glm::mat4 newMat = parentMat * b->matrix;

			glm::vec3 p1(parentMat * glm::vec4(0, 0, 0, 1));
			glm::vec3 p2(newMat * glm::vec4(0, 0, 0, 1));

			glm::vec3 p2_x(newMat * glm::vec4(1, 0, 0, 1));
			glm::vec3 p2_y(newMat * glm::vec4(0, 1, 0, 1));
			glm::vec3 p2_z(newMat * glm::vec4(0, 0, 1, 1));

			glColor3f(1, 1, 1);
			glVertex3f(p1.x, p1.y, p1.z);
			glVertex3f(p2.x, p2.y, p2.z);

			glColor3f(1, 0, 0);
			glVertex3f(p2.x, p2.y, p2.z);
			glVertex3f(p2_x.x, p2_x.y, p2_x.z);
			glColor3f(0, 1, 0);
			glVertex3f(p2.x, p2.y, p2.z);
			glVertex3f(p2_y.x, p2_y.y, p2_y.z);
			glColor3f(0, 0, 1);
			glVertex3f(p2.x, p2.y, p2.z);
			glVertex3f(p2_z.x, p2_z.y, p2_z.z);

			for (auto c : b->children)
				drawBone(c, newMat);
		};
/*		for (auto b : bones)
		{
			drawBone(b, glm::mat4());
		}*/

		drawBone(bones[0], glm::mat4());

		glEnd();
		glPopMatrix();
		glEnable(GL_DEPTH_TEST);

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
	template class LoLModel < gl::VertexP3N2B2T2T2B4B4 >;

}