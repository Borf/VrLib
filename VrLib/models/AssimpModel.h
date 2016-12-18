#pragma once

#include <VrLib/gl/VAO.h>
#include <VrLib/gl/VBO.h>
#include <VrLib/gl/VIO.h>
#include <VrLib/Model.h>
#include <functional>
#include <list>
#include <map>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

struct aiScene;
struct aiNode;

namespace vrlib
{
	class Animation;
	class AnimatedAssimpModel;

	class Bone
	{
	public:
		std::list<Bone*> children;
		Bone* parent = nullptr;
		std::string name;
		glm::mat4 matrix;

		int index = -1;
		glm::mat4 offset;
		glm::mat4 getMatrix(Animation* animation, float time) const;
		void update(std::vector<glm::mat4> &boneMatrices, float time, Animation* animation, const glm::mat4& parentMatrix = glm::mat4()) const;

		Bone* find(std::function<bool(Bone*)> callback);
	};

	class Animation
	{
	public:
		class Stream
		{
		public:
			template<class T>
			class Frame
			{
			public:
				float time;
				T value;

				Frame(float time, T value) { this->time = time; this->value = value; }
			};

			Stream(Bone* rootBone) { this->bone = rootBone; }

			Bone* bone;
			std::vector<Frame<glm::vec3> >		positions;
			std::vector < Frame<glm::vec3> >	scales;
			std::vector< Frame<glm::quat> >		rotations;
		};

		Animation::Stream* getStream(const Bone* bone);
		std::vector<Stream> streams;
		float totalTime;
		std::string name;
	};



	class State : public vrlib::ModelInstance
	{
	public:
		class AnimationState
		{
		public:
			double time;
			Animation* animation;
			float blendFactor;
			int playCount;
		};
		class Fader
		{
		public:
			AnimationState* animationState;
			float begin;
			float end;
			float time;
			float elapsedTime;
			bool stopWhenDone;
		};


		AnimatedAssimpModel* model;
		State(AnimatedAssimpModel* model);

		std::vector<glm::mat4> boneMatrices;
		std::vector<AnimationState*> animations;
		std::vector<Fader*> faders;


		void playAnimation(const std::string& animation, float fadeInTime = 0, bool playOnce = false);
		void stopAnimation(const std::string& animation, float fadeOutTime = 0);
		void update(double elapsedTime);
		virtual void draw(const std::function<void(const glm::mat4&)> &modelviewMatrixCallback, const std::function<void(const Material&)> &materialCallback = nullptr);
		void drawSkeleton();
	};


	class AnimatedAssimpModel : public Model
	{
	public:
		Bone* rootBone;
		std::vector<Bone*> bones;
		std::map<std::string, Animation*> animations;
		std::vector<State*> states;
	};

	template<class VertexFormat>
	class AssimpModel : public  AnimatedAssimpModel
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
			glm::mat4 localTransform;
			glm::mat4 globalTransform;
			Material material;
		};




		AssimpModel(const std::string &fileName, const ModelLoadOptions& options);

		void import(const glm::mat4 &matrix, const aiScene* scene, aiNode* node);
		Bone* buildSkeleton(const aiNode* node, Bone* parent = nullptr) const;
		virtual std::vector<glm::vec3> getVertices(int amount) const override;
		virtual std::vector<glm::vec3> getTriangles() const override;
		virtual std::pair<std::vector<unsigned int>, std::vector<glm::vec3>> getIndexedTriangles() const override;
		virtual void draw(const std::function<void(const glm::mat4&)> &modelviewMatrixCallback, const std::function<void(const vrlib::Material&)> &materialCallback) override;
		virtual ModelInstance* getInstance() override;
		virtual bool hasAlphaMaterial() override;
		virtual std::vector<vrlib::Material*> getMaterials() override;

		std::string path;

		std::vector<Mesh> meshes;
		std::vector<VertexFormat> vertices;
		std::vector<unsigned int> indices;

		gl::VBO<VertexFormat> vbo;
		gl::VIO<unsigned int> vio;
		gl::VAO* vao;






	};
}