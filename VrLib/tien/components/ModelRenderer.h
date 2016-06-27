#pragma once

#include "Renderable.h"
#include <string>
#include <map>
namespace vrlib
{
	class Model;
	class Texture;

	namespace tien
	{
		namespace components
		{
			class ModelRenderer : public Renderable
			{
			private:
				class ModelRenderContext : public Renderable::RenderContext, public Singleton<ModelRenderContext>
				{
				public:
					enum class RenderUniform
					{
						modelMatrix,
						projectionMatrix,
						viewMatrix,
						normalMatrix,
						s_texture,
						s_normalmap,
						diffuseColor,
						textureFactor,
					};
					vrlib::gl::Shader<RenderUniform>* renderShader;
					vrlib::Texture* defaultNormalMap;
					virtual void init() override;
					virtual void frameSetup(const glm::mat4 &projectionMatrix, const glm::mat4 &viewMatrix) override;
				};
				class ModelRenderShadowContext : public Renderable::RenderContext, public Singleton<ModelRenderShadowContext>
				{
				public:
					enum class RenderUniform
					{
						modelMatrix,
						projectionMatrix,
						viewMatrix,
						outputPosition
					};
					vrlib::gl::Shader<RenderUniform>* renderShader;
					virtual void init() override;
					virtual void frameSetup(const glm::mat4 &projectionMatrix, const glm::mat4 &viewMatrix) override;
					virtual void useCubemap(bool) override;
				};


				static std::map<std::string, vrlib::Model*> cache;
			public:
				ModelRenderer(const std::string &fileName);
				~ModelRenderer();

				vrlib::Model* model;

				void draw() override;
				void drawShadowMap() override;

				bool castShadow;
			};
		}
	}
}