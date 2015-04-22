#pragma once

#include <VrLib/Model.h>

namespace vrlib
{
	class SimpleModel : public Model
	{
	protected:
		SimpleModel(const std::string &fileName, const ModelLoadOptions &options = ModelLoadOptions());
	public:
		virtual std::vector<glm::vec3> getVertices(int amount) override;
		virtual void draw(gl::ShaderProgram* shader) override;
		virtual ModelInstance* getInstance() override;



		friend class Model;
	};



}