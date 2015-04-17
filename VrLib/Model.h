#pragma once


namespace vrlib
{
	namespace gl { class Shader; }
	class ModelLoadOptions
	{
	public:
		ModelLoadOptions();
		ModelLoadOptions(float size);
		ModelLoadOptions(float size, bool keepVerts);
	};

	class Model
	{
	private:
		Model();
	public:
		static Model* getModel(const std::string &fileName, const ModelLoadOptions &options = ModelLoadOptions()); 
		/*
			todo: could do this with overloading new
			http://stackoverflow.com/questions/24943831/pass-more-parameter-in-overriding-new-operatorc
			syntax would be something like
			Model* model = new("file.obj") Model();
			this kinda looks ugly though
		*/

		std::vector<glm::vec3> getVertices(int amount);
		void draw(gl::Shader* shader);
	};
}