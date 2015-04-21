#pragma once

#include <VrLib/gl/shader.h>

namespace vrlib
{
	class ModelLoadOptions
	{
	public:
		ModelLoadOptions() {};
		ModelLoadOptions(float size) {};
		ModelLoadOptions(float size, bool keepVerts) {};
	};

	class Model
	{
	private:
		Model();
	public:
		static Model* getModel(const std::string &fileName, const ModelLoadOptions &options = ModelLoadOptions()) { return NULL;  };
		/*
			todo: could do this with overloading new
			http://stackoverflow.com/questions/24943831/pass-more-parameter-in-overriding-new-operatorc
			syntax would be something like
			Model* model = new("file.obj") Model();
			this kinda looks ugly though
		*/

		std::vector<glm::vec3> getVertices(int amount) { return std::vector<glm::vec3>();  };
		void draw(gl::ShaderProgram* shader) {};
	};
}