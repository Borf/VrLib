#include "AnimatedModelRenderer.h"
#include <VrLib/Model.h>
#include <VrLib/Texture.h>
#include <VrLib/gl/Vertex.h>
#include "Transform.h"
#include "../Node.h"
#include <VrLib/models/AssimpModel.h>


namespace vrlib
{
	namespace tien
	{
		namespace components
		{
			std::map<std::string, vrlib::Model*> AnimatedModelRenderer::cache;

			AnimatedModelRenderer::AnimatedModelRenderer(const std::string &fileName)
			{
				if (cache.find(fileName) == cache.end())
					cache[fileName] = vrlib::Model::getModel<vrlib::gl::VertexP3N2B2T2T2B4B4>(fileName);
				model = cache[fileName];
				modelInstance = model->getInstance();
				renderContext = ModelRenderContext::getInstance();
				callbackOnDone = nullptr;
			}

			AnimatedModelRenderer::~AnimatedModelRenderer()
			{

			}


			void AnimatedModelRenderer::update(float elapsedTime, Scene& scene)
			{
				modelInstance->update(elapsedTime);

				if (callbackOnDone)
				{
					if (((vrlib::State*)modelInstance)->animations.size() == 0)
					{
						callbackOnDone();
						callbackOnDone = nullptr;
					}
				}

			}

			void AnimatedModelRenderer::draw()
			{
				components::Transform* t = node->getComponent<Transform>();

				ModelRenderContext* context = dynamic_cast<ModelRenderContext*>(renderContext);
				context->renderShader->use(); //TODO: only call this once!
				//TODO: ewwww
				context->renderShader->setUniform(ModelRenderContext::RenderUniform::boneMatrices, ((vrlib::State*)modelInstance)->boneMatrices);

				modelInstance->draw([this, t, &context](const glm::mat4 &modelMatrix)
				{
					context->renderShader->setUniform(ModelRenderContext::RenderUniform::modelMatrix, t->globalTransform);
					context->renderShader->setUniform(ModelRenderContext::RenderUniform::normalMatrix, glm::transpose(glm::inverse(glm::mat3(t->globalTransform))));
				},
					[this, &context](const vrlib::Material &material)
				{
					if (material.texture)
					{
						context->renderShader->setUniform(ModelRenderContext::RenderUniform::textureFactor, 1.0f);
						material.texture->bind();
						glActiveTexture(GL_TEXTURE1);
						if (material.normalmap)
							material.normalmap->bind();
						else
							context->defaultNormalMap->bind();
						glActiveTexture(GL_TEXTURE0);

					}
					else
					{
						context->renderShader->setUniform(ModelRenderContext::RenderUniform::textureFactor, 0.0f);
						context->renderShader->setUniform(ModelRenderContext::RenderUniform::diffuseColor, material.color.diffuse);
					}
				});
			}



			void AnimatedModelRenderer::playAnimation(const std::string &animation, bool loop)
			{
				((vrlib::State*)modelInstance)->playAnimation(animation, 0, !loop);
			}

			void AnimatedModelRenderer::playAnimation(const std::string &animation, std::function<void()> callbackOnDone)
			{
				this->callbackOnDone = callbackOnDone;
				((vrlib::State*)modelInstance)->playAnimation(animation, 0, true);
			}

			void AnimatedModelRenderer::ModelRenderContext::init()
			{
				renderShader = new vrlib::gl::Shader<RenderUniform>("data/vrlib/tien/shaders/animatedModel.vert", "data/vrlib/tien/shaders/animatedModel.frag");
				renderShader->bindAttributeLocation("a_position", 0);
				renderShader->bindAttributeLocation("a_normal", 1);
				renderShader->bindAttributeLocation("a_bitangent", 2);
				renderShader->bindAttributeLocation("a_tangent", 3);
				renderShader->bindAttributeLocation("a_texture", 4);
				renderShader->bindAttributeLocation("a_boneIds", 5);
				renderShader->bindAttributeLocation("a_boneWeights", 6);
				renderShader->link();
				renderShader->bindFragLocation("fragColor", 0);
				renderShader->bindFragLocation("fragNormal", 1);
				//shader->bindFragLocation("fragColor", 0);
				renderShader->registerUniform(RenderUniform::modelMatrix, "modelMatrix");
				renderShader->registerUniform(RenderUniform::projectionMatrix, "projectionMatrix");
				renderShader->registerUniform(RenderUniform::viewMatrix, "viewMatrix");
				renderShader->registerUniform(RenderUniform::normalMatrix, "normalMatrix");
				renderShader->registerUniform(RenderUniform::s_texture, "s_texture");
				renderShader->registerUniform(RenderUniform::s_normalmap, "s_normalmap");
				renderShader->registerUniform(RenderUniform::diffuseColor, "diffuseColor");
				renderShader->registerUniform(RenderUniform::textureFactor, "textureFactor");
				renderShader->registerUniform(RenderUniform::boneMatrices, "boneMatrices");
				renderShader->use();
				renderShader->setUniform(RenderUniform::s_texture, 0);
				renderShader->setUniform(RenderUniform::s_normalmap, 1);

				defaultNormalMap = vrlib::Texture::loadCached("data/vrlib/tien/textures/defaultnormalmap.png");

			}

			void AnimatedModelRenderer::ModelRenderContext::frameSetup(const glm::mat4 & projectionMatrix, const glm::mat4 & viewMatrix)
			{
				renderShader->use();
				renderShader->setUniform(RenderUniform::projectionMatrix, projectionMatrix);
				renderShader->setUniform(RenderUniform::viewMatrix, viewMatrix);
				renderShader->setUniform(RenderUniform::diffuseColor, glm::vec4(1, 1, 1, 1));
				renderShader->setUniform(RenderUniform::textureFactor, 1.0f);
			}

		}
	}
}