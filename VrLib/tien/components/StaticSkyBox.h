#pragma once

#include "SkyBox.h"
#include <VrLib/gl/shader.h>
#include <string>

namespace vrlib
{
	class Model;
	class Texture;
	namespace gl { class CubeMap; }
	namespace tien
	{
		namespace components
		{
			class StaticSkyBox : public SkyBox
			{
				enum class Uniforms
				{
					modelViewMatrix,
					projectionMatrix
				};
				vrlib::gl::Shader<Uniforms>* shader;
				vrlib::Model* skybox;
				vrlib::gl::CubeMap* cubemap;

				vrlib::Texture* skydomeGlow;
			public:
				virtual void initialize() override;
				virtual void update(float elapsedTime, Scene& scene) override;
				virtual void render(const glm::mat4 & projectionMatrix, const glm::mat4 & modelviewMatrix) override;
				virtual json::Value toJson() const;
				virtual void setTexture(int side, const std::string &texture);
			};
		}
	}
}