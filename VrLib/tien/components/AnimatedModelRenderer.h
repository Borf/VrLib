#pragma once

#include "Renderable.h"
#include <string>
#include <map>
#include <functional>

namespace vrlib
{
	class Model;
	class ModelInstance;
	class Texture;

	namespace tien
	{
		namespace components
		{
			class AnimatedModelRenderer : public Renderable
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
						boneMatrices
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
						boneMatrices,
						outputPosition
					};
					vrlib::gl::Shader<RenderUniform>* renderShader;
					virtual void init() override;
					virtual void frameSetup(const glm::mat4 &projectionMatrix, const glm::mat4 &viewMatrix) override;
					virtual void useCubemap(bool use) override;
				};

				std::function<void()> callbackOnDone;

				static std::map<std::string, vrlib::Model*> cache;
			public:
				AnimatedModelRenderer(const std::string &fileName);
				~AnimatedModelRenderer();

				vrlib::Model* model;
				vrlib::ModelInstance* modelInstance;
				
				
				float animationSpeed = 1;

				void update(float elapsedTime, Scene& scene) override;
				void draw() override;
				void drawShadowMap() override;

				void playAnimation(const std::string &animation, bool loop = true);
				void playAnimation(const std::string &animation, std::function<void()> callbackOnDone);



				bool castShadow;
			};
		}
	}
}