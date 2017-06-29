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
						hasNormalMap
					};
					vrlib::Texture* white;
					vrlib::gl::Shader<RenderUniform>* renderShader;
					vrlib::Texture* defaultNormalMap;
					virtual void init() override;
					virtual void frameSetup(const glm::mat4 &projectionMatrix, const glm::mat4 &viewMatrix) override;
				};

				class ModelForwardRenderContext : public Renderable::RenderContext, public MapSingleton<ModelForwardRenderContext, std::string>
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
					std::string shaderFile;
					ModelForwardRenderContext(const std::string &shader);
					//ModelForwardRenderContext();
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

				vrlib::Model* prevModel = nullptr;

				std::string fileName;
				static std::map<std::string, vrlib::Model*> cache;
				bool hasForward = false;
			public:
				ModelRenderer(const json &json);
//				ModelRenderer(const std::string &fileName);
				~ModelRenderer();

				vrlib::Model* model;
				std::string getFileName()
				{
					return fileName;
				}

				virtual void update(float elapsedTime, Scene& scene) override;
				void drawDeferredPass() override;
				void drawForwardPass() override;
				void drawShadowMap() override;
				json toJson(json &meshes) const override;

				std::string getFileName() {
					return fileName;
				}

				bool castShadow;
				bool cullBackFaces;
				std::map<vrlib::Material*, vrlib::Material> materialOverrides;
				Material* getCurrentMaterial();
				virtual void buildEditor(EditorBuilder* builder, bool folded) override;

			};
		}
	}
}