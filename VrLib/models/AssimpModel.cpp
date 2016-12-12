#include "AssimpModel.h"

#include <VrLib/gl/Vertex.h>
#include <VrLib/Texture.h>
#include <VrLib/Log.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

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
				//logger<<"Mesh does not have normals...calculating" << Log::newline;
				for (unsigned int ii = 0; ii < mesh->mNumFaces; ii++)
				{
					const struct aiFace* face = &mesh->mFaces[ii];
					if (face->mNumIndices > 2)
					{
						glm::vec3 v1(mesh->mVertices[face->mIndices[0]].x, mesh->mVertices[face->mIndices[0]].y, mesh->mVertices[face->mIndices[0]].z);
						glm::vec3 v2(mesh->mVertices[face->mIndices[1]].x, mesh->mVertices[face->mIndices[1]].y, mesh->mVertices[face->mIndices[1]].z);
						glm::vec3 v3(mesh->mVertices[face->mIndices[2]].x, mesh->mVertices[face->mIndices[2]].y, mesh->mVertices[face->mIndices[2]].z);
						//v1 = glm::vec3(glm::vec4(v1, 1) * transform);
						//v2 = glm::vec3(glm::vec4(v2, 1) * transform);
						//v3 = glm::vec3(glm::vec4(v3, 1) * transform);

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
					setT2(v, glm::vec2(mesh->mTextureCoords[0][ii].x, mesh->mTextureCoords[0][ii].y));

				glm::vec4 normal;
				if (mesh->HasNormals())
					setN3(v, glm::vec3(mesh->mNormals[ii].x, mesh->mNormals[ii].y, mesh->mNormals[ii].z)); //matrix?
				else
					setN3(v, vertexNormals[ii]); //matrix?
//					setN3(v, glm::vec3(0, 0, 1));

				if (mesh->HasTangentsAndBitangents())
				{
					gl::setTan3(v, glm::normalize(glm::vec3(mesh->mTangents[ii].x, mesh->mTangents[ii].y, mesh->mTangents[ii].z)));
					gl::setBiTan3(v, glm::normalize(glm::vec3(mesh->mBitangents[ii].x, mesh->mBitangents[ii].y, mesh->mBitangents[ii].z)));
				}



				vertices.push_back(v);
			}

			if (mesh->HasBones())
			{
				for (unsigned int ii = 0; ii < mesh->mNumBones; ii++)
				{
					const aiBone* bone = mesh->mBones[ii];
					for (unsigned int iii = 0; iii < bone->mNumWeights; iii++)	// setup the vertices
					{
						const aiVertexWeight& weight = bone->mWeights[iii];
						int index = (vertexStart + weight.mVertexId);
						for (unsigned int iiii = 0; iiii < 4; iiii++)
						{
							if(vrlib::gl::getBoneId(vertices[index], iiii) == -1)
							{
							vrlib::gl::setB4(vertices[index], iiii, ii, weight.mWeight);
							break; //TODO: if index iiii is already filled in, don't overwrite it, but use proper weights etc
							}
							assert(iiii != 3);
						}
					}

					std::string name = bone->mName.C_Str();

					Bone* b = rootBone->find([&name](Bone* c) { return c->name == name; });
					if (b)
					{
						//TODO: this could be causing problems, if b->index is already set and it's different from ii
						b->index = ii;
						b->offset = glm::transpose(glm::make_mat4((float*)&bone->mOffsetMatrix));
					}
					if (bones.size() <= ii)
						bones.resize(ii + 1, nullptr);
					bones[ii] = b;

				}
			}
			
			Mesh m;
			m.indexStart = indices.size();
			for (unsigned int ii = 0; ii < mesh->mNumFaces; ii++)
			{
				const struct aiFace* face = &mesh->mFaces[ii];
				if (face->mNumIndices != 3)
					continue;

				for (int iii = 0; iii < 3; iii++)
				{
					assert(face->mIndices[iii] >= 0 && face->mIndices[iii] < mesh->mNumVertices);
					indices.push_back(vertexStart + (int)face->mIndices[iii]);
				}
			}
			m.indexCount = indices.size() - m.indexStart;
			m.localTransform = glm::transpose(glm::make_mat4((float*)&node->mTransformation));
			m.globalTransform = transform;

			
			aiString texPath;
			
			if (scene->mMaterials[mesh->mMaterialIndex]->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == aiReturn_SUCCESS)
			{
				std::string file = texPath.C_Str();
				m.material.texture = vrlib::Texture::loadCached(path + "/" + file);
				while (!m.material.texture && file.find("/") != std::string::npos)
				{
					file = file.substr(file.find("/") + 1);
					m.material.texture = vrlib::Texture::loadCached(path + "/" + file);
				}
				while(!m.material.texture && file.find("\\") != std::string::npos)
				{
					file = file.substr(file.find("\\") + 1);
					m.material.texture = vrlib::Texture::loadCached(path + "/" + file);
				}
			}
			else
				m.material.texture = NULL;


			if (scene->mMaterials[mesh->mMaterialIndex]->GetTexture(aiTextureType_HEIGHT, 0, &texPath) == aiReturn_SUCCESS)
			{
				std::string file = texPath.C_Str();
				m.material.normalmap = vrlib::Texture::loadCached(path + "/" + file);
				while (!m.material.normalmap && file.find("/") != std::string::npos)
				{
					file = file.substr(file.find("/") + 1);
					m.material.normalmap = vrlib::Texture::loadCached(path + "/" + file);
				}
				while (!m.material.normalmap && file.find("\\") != std::string::npos)
				{
					file = file.substr(file.find("\\") + 1);
					m.material.normalmap = vrlib::Texture::loadCached(path + "/" + file);
				}
			}
			else
				m.material.normalmap = NULL;

			if (scene->mMaterials[mesh->mMaterialIndex]->GetTexture(aiTextureType_NORMALS, 0, &texPath) == aiReturn_SUCCESS)
			{
				std::string file = texPath.C_Str();
				m.material.normalmap = vrlib::Texture::loadCached(path + "/" + file);
				while (!m.material.normalmap && file.find("/") != std::string::npos)
				{
					file = file.substr(file.find("/") + 1);
					m.material.normalmap = vrlib::Texture::loadCached(path + "/" + file);
				}
				while (!m.material.normalmap && file.find("\\") != std::string::npos)
				{
					file = file.substr(file.find("\\") + 1);
					m.material.normalmap = vrlib::Texture::loadCached(path + "/" + file);
				}
			}



			aiColor4D color;
			if (aiGetMaterialColor(scene->mMaterials[mesh->mMaterialIndex], AI_MATKEY_COLOR_AMBIENT, &color) == aiReturn_SUCCESS)
				m.material.color.ambient = glm::vec4(color.r, color.g, color.b, color.a);
			if (aiGetMaterialColor(scene->mMaterials[mesh->mMaterialIndex], AI_MATKEY_COLOR_DIFFUSE, &color) == aiReturn_SUCCESS)
				m.material.color.diffuse = glm::vec4(color.r, color.g, color.b, color.a);
			if (aiGetMaterialColor(scene->mMaterials[mesh->mMaterialIndex], AI_MATKEY_COLOR_SPECULAR, &color) == aiReturn_SUCCESS)
				m.material.color.specular = glm::vec4(color.r, color.g, color.b, color.a);

			float opacity;
			if (aiGetMaterialFloat(scene->mMaterials[mesh->mMaterialIndex], AI_MATKEY_OPACITY, &opacity) == aiReturn_SUCCESS)
				m.material.color.diffuse.a = opacity;

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
		vao = NULL;
		Assimp::Importer importer;
		aiScene* scene = NULL;
		try {
			scene = (aiScene*)importer.ReadFile(fileName, aiProcessPreset_TargetRealtime_Quality);
			if (!scene)
			{
				logger<<"Error loading file : " << importer.GetErrorString() << Log::newline;
				return;
			}
		} catch (char* e) {
			logger << "Error loading file : " << importer.GetErrorString() << Log::newline;
			logger << e << Log::newline;
			return;
		}

		path = "";
		if (fileName.find("/") != std::string::npos)
			path = fileName.substr(0, fileName.rfind("/"));
		if (fileName.find("\\") != std::string::npos)
			path = fileName.substr(0, fileName.rfind("\\"));

		
		int numVerts = 0;
		int numIndices = 0;
		for (size_t i = 0; i < scene->mNumMeshes; i++)
		{
			numVerts += scene->mMeshes[i]->mNumVertices;
			numIndices += scene->mMeshes[i]->mNumFaces * 3;
		}

		vertices.reserve(numVerts);
		indices.reserve(numIndices);
		rootBone = buildSkeleton(scene->mRootNode);
		import(glm::mat4(), scene, scene->mRootNode);


		if (scene->HasAnimations())
		{
			for (unsigned int i = 0; i < scene->mNumAnimations; i++)
			{
				const aiAnimation* animation = scene->mAnimations[i];
				float tps = (float)animation->mTicksPerSecond;
				if (tps == 0) tps = 25;				
				Animation* a = new Animation();
				a->name = animation->mName.C_Str();
				a->totalTime = (float)(animation->mDuration / tps);

				for (unsigned int ii = 0; ii < animation->mNumChannels; ii++)
				{
					const aiNodeAnim* stream = animation->mChannels[ii];
					Animation::Stream s(rootBone->find([stream](Bone* b) { return b->name == stream->mNodeName.C_Str();  }));
					for (unsigned int iii = 0; iii < stream->mNumPositionKeys; iii++)
						s.positions.push_back(Animation::Stream::Frame<glm::vec3>((float)(stream->mPositionKeys[iii].mTime / tps), glm::vec3(stream->mPositionKeys[iii].mValue.x, stream->mPositionKeys[iii].mValue.y, stream->mPositionKeys[iii].mValue.z)));
					for (unsigned int iii = 0; iii < stream->mNumScalingKeys; iii++)
						s.scales.push_back(Animation::Stream::Frame<glm::vec3>((float)(stream->mScalingKeys[iii].mTime / tps), glm::vec3(stream->mScalingKeys[iii].mValue.x, stream->mScalingKeys[iii].mValue.y, stream->mScalingKeys[iii].mValue.z)));
					for (unsigned int iii = 0; iii < stream->mNumRotationKeys; iii++)
						s.rotations.push_back(Animation::Stream::Frame<glm::quat>((float)(stream->mRotationKeys[iii].mTime / tps), glm::quat(stream->mRotationKeys[iii].mValue.w, stream->mRotationKeys[iii].mValue.x, stream->mRotationKeys[iii].mValue.y, stream->mRotationKeys[iii].mValue.z)));


					a->streams.push_back(s);
				}


				animations[a->name] = a;
			}
		}

		handleModelLoadOptions(vertices, options);


		vbo.bind();
		vbo.setData(vertices.size(), &vertices[0], GL_STATIC_DRAW);

		vao = new gl::VAO(&vbo);
		vao->bind();
		vio.bind();
		vio.setData(indices.size(), &indices[0], GL_STATIC_DRAW);

		vao->unBind();

//		indices.clear();
//		vertices.clear();
	}

	template<class T>
	Bone* AssimpModel<T>::buildSkeleton(const aiNode* node, Bone* parent) const
	{
		Bone* bone = new Bone();
		bone->parent = parent;
		bone->name = node->mName.C_Str();
		bone->matrix = glm::transpose(glm::make_mat4((float*)&node->mTransformation));
		for (unsigned int i = 0; i < node->mNumChildren; i++)
			bone->children.push_back(buildSkeleton(node->mChildren[i]));
		return bone;
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
				v = glm::vec3(mesh.globalTransform * glm::vec4(v, 1));
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
				v = glm::vec3(mesh.globalTransform * glm::vec4(v, 1));
				ret.push_back(glm::vec3(v.x, v.y, v.z));
			}
		}
		return ret;
	}


	template<class VertexFormat>
	std::pair<std::vector<unsigned int>, std::vector<glm::vec3>> AssimpModel<VertexFormat>::getIndexedTriangles() const
	{
		std::pair<std::vector<unsigned int>, std::vector<glm::vec3>> ret;
		for (const Mesh& mesh : meshes)
		{
			for (int i = mesh.indexStart; i < mesh.indexStart + mesh.indexCount; i++)
			{
				glm::vec3 v = glm::make_vec3(&vertices[indices[i]].px);
				v = glm::vec3(mesh.globalTransform * glm::vec4(v, 1));
				ret.first.push_back(ret.first.size());
				ret.second.push_back(glm::vec3(v.x, v.y, v.z));
			}
		}
//TODO: return properly
		return ret;
	}



	template<class VertexFormat>
	void AssimpModel<VertexFormat>::draw(const std::function<void(const glm::mat4&)> &modelviewMatrixCallback, const std::function<void(const vrlib::Material&)> &materialCallback)
	{
		vao->bind();
		for (const Mesh& mesh : meshes)
		{
			if (modelviewMatrixCallback)
				modelviewMatrixCallback(mesh.globalTransform);
			if (materialCallback)
				materialCallback(mesh.material);
			else
			{
				if (mesh.material.texture)
					mesh.material.texture->bind();
			}
			if (mesh.indexCount == 0)
				continue;
			glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, (void*)(mesh.indexStart * sizeof(unsigned int)));
		}
		vao->unBind();
	}

	template<class VertexFormat>
	ModelInstance* AssimpModel<VertexFormat>::getInstance()
	{
		return new State(this);
	}

	template<class VertexFormat>
	bool AssimpModel<VertexFormat>::hasAlphaMaterial()
	{
		for (const auto &mesh : meshes)
		{
			if (mesh.material.texture)
			{
				if (mesh.material.texture->usesAlphaChannel)
					return true;
			}
		}
		return false;
	}



	////////animation stuff


	State::State(AnimatedAssimpModel* model) : vrlib::ModelInstance(model)
	{
		this->model = model;
		boneMatrices.resize(model->bones.size());
		
		//static_cast<AssimpModel<vrlib::gl::VertexP3N3T2B4B4>*>(model)->bones;
	}


	void State::update(double elapsedTime)
	{
		if (animations.empty())
		{
			model->rootBone->update(this->boneMatrices, 0, NULL);
		}
		for (auto a : animations)
		{
			a->time += elapsedTime;
			if (a->playCount != 0)
				if (a->time > a->animation->totalTime)
					a->playCount--;
			a->time = fmod(a->time, a->animation->totalTime);
		}
		animations.erase(std::remove_if(animations.begin(), animations.end(), [](AnimationState* a) { return a->playCount == 0;  }), animations.end());



		for (int i = 0; i < (int)faders.size(); i++)
		{
			faders[i]->elapsedTime += (float)elapsedTime;
			float fac = glm::min(1.0f, faders[i]->elapsedTime / faders[i]->time);
			faders[i]->animationState->blendFactor = faders[i]->begin + fac * (faders[i]->end - faders[i]->begin);
			if (faders[i]->elapsedTime > faders[i]->time)
			{
				if (faders[i]->stopWhenDone)
				{
					for (size_t ii = 0; ii < animations.size(); ii++)
						if (animations[ii] == faders[i]->animationState)
							animations.erase(animations.begin() + ii);
					delete faders[i]->animationState;
				}
				delete faders[i];
				faders.erase(faders.begin() + i);
				i--;
			}
		}


		if(!animations.empty())
			model->rootBone->update(this->boneMatrices, (float)animations[0]->time, animations[0]->animation);

/*		model->rootBone->update(this->boneMatrices, 0, NULL);
		float totalFac = 0;//TODO blib::linq::sum<float>(animations, [](AnimationState* s) { return s->blendFactor; });
		for (size_t ii = 0; ii < boneMatrices.size(); ii++)
			boneMatrices[ii] = boneMatrices[ii] * (1 - totalFac);
		std::vector<glm::mat4> tmpMatrices = boneMatrices;
		for (size_t i = 0; i < animations.size(); i++)
		{
			model->rootBone->update(tmpMatrices, (float)animations[i]->time, animations[i]->animation);
			for (size_t ii = 0; ii < boneMatrices.size(); ii++)
			{
				boneMatrices[ii] += tmpMatrices[ii] * animations[i]->blendFactor;
			}
		}*/
	}

	void State::draw(const std::function<void(const glm::mat4&)>& modelviewMatrixCallback, const std::function<void(const Material&)>& materialCallback)
	{
		model->draw(modelviewMatrixCallback, materialCallback);


#if 0
		glColor4f(1, 1, 1, 1);
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_DEPTH_TEST);
		glUseProgram(0);
		glPushMatrix();
		glLineWidth(1);
		glBegin(GL_LINES);

		std::function<void(Bone*, const glm::mat4 &)> drawStuff;
		drawStuff = [&drawStuff, this](Bone* bone, const glm::mat4 &parentMatrix)
		{
			glm::mat4 mat = parentMatrix * bone->getMatrix(animations[0]->animation, animations[0]->time);// parentMatrix * bone->matrix;
			//glm::mat4 mat = parentMatrix * bone->getMatrix(nullptr, 0);// parentMatrix * bone->matrix;
			glm::vec4 p2(parentMatrix * glm::vec4(0, 0, 0, 1));
			glm::vec4 p1(mat * glm::vec4(0, 0, 0, 1));

			glm::vec3 px(mat * glm::vec4(.1f, 0, 0, 1));
			glm::vec3 py(mat * glm::vec4(0, .1f, 0, 1));
			glm::vec3 pz(mat * glm::vec4(0, 0, .1f, 1));

			glColor3f(1, 1, 1);
			glVertex3f(p1.x, p1.y, p1.z);
			glVertex3f(p2.x, p2.y, p2.z);
			glColor3f(1, 0, 0);
			glVertex3f(p1.x, p1.y, p1.z);
			glVertex3f(px.x, px.y, px.z);
			glColor3f(0, 1, 0);
			glVertex3f(p1.x, p1.y, p1.z);
			glVertex3f(py.x, py.y, py.z);
			glColor3f(0, 0, 1);
			glVertex3f(p1.x, p1.y, p1.z);
			glVertex3f(pz.x, pz.y, pz.z);

			for (auto c : bone->children)
				drawStuff(c, mat);

		};

		drawStuff(this->model->rootBone, glm::mat4());

		glEnd();

#endif

	}



	void Bone::update(std::vector<glm::mat4> &boneMatrices, float time, Animation* animation, const glm::mat4& parentMatrix) const
	{
		glm::mat4 globalMatrix = parentMatrix * getMatrix(animation, time);
		if (index >= 0)
			boneMatrices[index] = globalMatrix * offset;
		for (auto c : children)
			c->update(boneMatrices, time, animation, globalMatrix);
	}

	glm::mat4 Bone::getMatrix(Animation* animation, float time) const
	{
		if (!animation)
			return matrix;
		Animation::Stream* s = animation->getStream(this); //TODO: cache this?
		if (!s)
			return matrix;

		glm::vec3 pos;
		glm::vec3 scale;
		glm::quat rot;
		{
			unsigned int rotFrameIndex = 0;
			while (rotFrameIndex < s->rotations.size() - 1 && time > s->rotations[rotFrameIndex + 1].time)
				rotFrameIndex++;
			int next = (rotFrameIndex + 1) % s->rotations.size();
			float timeDiff = s->rotations[next].time - s->rotations[rotFrameIndex].time;
			float offset = time - s->rotations[rotFrameIndex].time;
			float fac = offset / timeDiff;
			if (timeDiff == 0)
				fac = 0;
			assert(fac >= 0 && fac <= 1);
			rot = glm::slerp(s->rotations[rotFrameIndex].value, s->rotations[next].value, fac);
		}

		{
			unsigned int posFrameIndex = 0;
			while (posFrameIndex < s->positions.size() - 1 && time > s->positions[posFrameIndex + 1].time)
				posFrameIndex++;
			int next = (posFrameIndex + 1) % s->positions.size();
			float timeDiff = s->positions[next].time - s->positions[posFrameIndex].time;
			float offset = time - s->positions[posFrameIndex].time;
			float fac = offset / timeDiff;
			if (timeDiff == 0)
				fac = 0;
			assert(fac >= 0 && fac <= 1);
			pos = glm::mix(s->positions[posFrameIndex].value, s->positions[next].value, fac);
		}

		{
			unsigned int scaleFrameIndex = 0;
			while (scaleFrameIndex < s->scales.size() - 1 && time > s->scales[scaleFrameIndex + 1].time)
				scaleFrameIndex++;
			int next = (scaleFrameIndex + 1) % s->scales.size();
			float timeDiff = s->scales[next].time - s->scales[scaleFrameIndex].time;
			float offset = time - s->scales[scaleFrameIndex].time;
			float fac = offset / timeDiff;
			if (timeDiff == 0)
				fac = 0;
			assert(fac >= 0 && fac <= 1);
			scale = glm::mix(s->scales[scaleFrameIndex].value, s->scales[next].value, fac);
		}
		glm::mat4 animMatrix;
		animMatrix = glm::translate(animMatrix, pos);
		animMatrix = animMatrix * glm::toMat4(rot);
		animMatrix = glm::scale(animMatrix, scale);

		return animMatrix;
	}

	Animation::Stream* Animation::getStream(const Bone* bone)
	{
		for (size_t i = 0; i < streams.size(); i++)
			if (streams[i].bone == bone)
				return &streams[i];
		return NULL;
	}
	void State::playAnimation(const std::string& animation, float fadeInTime, bool playOnce)
	{
		for (size_t i = 0; i < animations.size(); i++)
			if (animations[i]->animation->name == animation)
				return;
		if (model->animations.find(animation) == model->animations.end())
		{
			logger << "Could not find animation " << animation << Log::newline <<"Animations ("<<model->animations.size()<<"): ";
			for (auto a : model->animations)
				logger << a.first << ", ";
			logger << Log::newline;

			return;
		}

		AnimationState* anim = new AnimationState();
		anim->animation = model->animations[animation];
		anim->blendFactor = fadeInTime == 0 ? 1.0f : 0.0f;
		anim->playCount = playOnce ? 1 : -1;
		anim->time = 0;
		animations.push_back(anim);

		if (fadeInTime > 0)
		{
			Fader* fader = new Fader();
			fader->animationState = anim;
			fader->begin = 0;
			fader->end = 1;
			fader->time = fadeInTime;
			fader->elapsedTime = 0;
			fader->stopWhenDone = false;
			faders.push_back(fader);
		}


	}

	void State::stopAnimation(const std::string& animation, float fadeOutTime)
	{
		for (int i = 0; i < (int)faders.size(); i++)
		{
			if (faders[i]->animationState->animation->name == animation && faders[i]->end == 0)
				return;
			if (faders[i]->animationState->animation->name == animation)
			{
				delete faders[i];
				faders.erase(faders.begin() + i);
				i--;
				continue;
			}

		}

		for (size_t i = 0; i < animations.size(); i++)
		{
			if (animations[i]->animation->name == animation)
			{
				if (fadeOutTime > 0)
				{
					Fader* fader = new Fader();
					fader->animationState = animations[i];
					fader->begin = 1;
					fader->end = 0;
					fader->time = fadeOutTime;
					fader->elapsedTime = 0;
					fader->stopWhenDone = true;
					faders.push_back(fader);
					return;
				}
				//		delete animations[i]; TODO: memory leak !
				animations.erase(animations.begin() + i);
				return;
			}
		}
	}



	template class AssimpModel < gl::VertexP3 > ;
	template class AssimpModel < gl::VertexP3N3 >;
	template class AssimpModel < gl::VertexP3N3T2 >;
	template class AssimpModel < gl::VertexP3N3T2B4B4 >;
	template class AssimpModel < gl::VertexP3N2B2T2T2 >;
	template class AssimpModel < gl::VertexP3N2B2T2T2B4B4 >;

	inline Bone * Bone::find(std::function<bool(Bone*)> callback)
	{
		if (callback(this))
			return this;
		for (auto b : children)
		{
			Bone* f = b->find(callback);
			if (f)
				return f;
		}
		return nullptr;
	}


}