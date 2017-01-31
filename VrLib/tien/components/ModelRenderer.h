#pragma once

#include "Renderable.h"
#include <string>
#include <map>
#include <vrlib/util/optional> //TODO: move this to std::optional when available

namespace vrlib
{
	class Model;
	class Texture;
	class Material;

	namespace tien
	{
		namespace components
		{
			class ModelRenderer : public Renderable
			{
			private:
				class ModelDeferredRenderContext : public Renderable::RenderContext, public Singleton<ModelDeferredRenderContext>
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
						s_specularmap,
						diffuseColor,
						textureFactor,
						shinyness,
					};
					vrlib::Texture* white;
					vrlib::gl::Shader<RenderUniform>* renderShader;
					vrlib::Texture* defaultNormalMap;
					virtual void init() override;
					virtual void frameSetup(const glm::mat4 &projectionMatrix, const glm::mat4 &viewMatrix) override;
				};
				class ModelForwardRenderContext : public Renderable::RenderContext, public Singleton<ModelForwardRenderContext>
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
						s_specularmap,
						diffuseColor,
						textureFactor,
						shinyness,
					};
					vrlib::gl::Shader<RenderUniform>* renderShader;
					vrlib::Texture* defaultNormalMap;
					virtual void init() override;
					virtual void frameSetup(const glm::mat4 &projectionMatrix, const glm::mat4 &viewMatrix) override;
				};
				class ModelShadowRenderContext : public Renderable::RenderContext, public Singleton<ModelShadowRenderContext>
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

				std::map<vrlib::Material*, vrlib::Material> materialOverrides;
				vrlib::Model* prevModel = nullptr;

				std::string fileName;
				static std::map<std::string, vrlib::Model*> cache;
				bool hasForward = false;
			public:
				ModelRenderer(const vrlib::json::Value &json);
//				ModelRenderer(const std::string &fileName);
				~ModelRenderer();

				vrlib::Model* model;
				virtual void update(float elapsedTime, Scene& scene) override;
				void drawDeferredPass() override;
				void drawForwardPass() override;
				void drawShadowMap() override;
				json::Value toJson(json::Value &meshes) const override;

				bool castShadow;
				bool cullBackFaces;
				virtual void buildEditor(EditorBuilder* builder, bool folded) override;

			};
		}
	}
}