#pragma once

#include "SkyBox.h"
#include <VrLib/gl/shader.h>

namespace vrlib
{
	class Model;
	class Texture;
	namespace tien
	{
		namespace components
		{
			// http://csc.lsu.edu/~kooima/misc/cs594/final/index.html
			class DynamicSkyBox : public SkyBox
			{
				enum class SkydomeUniforms
				{
					modelViewMatrix,
					projectionMatrix,
					glow,
					color,
					sunDirection,
				};
				vrlib::gl::Shader<SkydomeUniforms>* skydomeShader;
				vrlib::Model* skydome;
				vrlib::Texture* skydomeColor;
				vrlib::Texture* skydomeGlow;

				enum class BillboardUniforms
				{
					projectionMatrix,
					mat,
					s_texture
				};
				vrlib::gl::Shader<BillboardUniforms>* billboardShader;
				vrlib::Model* sun;
				vrlib::Model* moon;
			public:
				DynamicSkyBox() {};
				DynamicSkyBox(const vrlib::json::Value &json);
				virtual void initialize() override;
				virtual void update(float elapsedTime, Scene& scene) override;
				virtual void render(const glm::mat4 & projectionMatrix, const glm::mat4 & modelviewMatrix) override;


				virtual void buildEditor(EditorBuilder * builder, bool folded) override;

				Node* light = nullptr;
				float timeOfDay = 14;
				virtual json::Value toJson(json::Value &meshes) const override;
			};
		}
	}
}