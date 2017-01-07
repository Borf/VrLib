#include "AnimatedModelRenderer.h"
#include <VrLib/Model.h>
#include <VrLib/Texture.h>
#include <VrLib/gl/Vertex.h>
#include "Transform.h"
#include "../Node.h"
#include <VrLib/models/AssimpModel.h>
#include <VrLib/json.h>
#include <VrLib/Image.h>


namespace vrlib
{
	namespace tien
	{
		namespace components
		{
			std::map<std::string, vrlib::Model*> AnimatedModelRenderer::cache;

			AnimatedModelRenderer::AnimatedModelRenderer(const vrlib::json::Value &json)
			{
				if (json.isString())
					fileName = json;
				else
					fileName = json["file"];

				if (cache.find(fileName) == cache.end())
					cache[fileName] = vrlib::Model::getModel<vrlib::gl::VertexP3N2B2T2T2B4B4>(fileName);
				model = cache[fileName];
				if (!model)
					return;
				modelInstance = model->getInstance();
				renderContextDeferred = ModelRenderContext::getInstance();
				renderContextShadow = ModelShadowRenderContext::getInstance();
				callbackOnDone = nullptr;
				if (json.isObject())
				{
					castShadow = json["castShadow"];
					cullBackFaces = json["cullBackFaces"];
					if (json.isMember("animation"))
						this->playAnimation(json["animation"]);
				}
				else
				{
					castShadow = true;
					cullBackFaces = true;
				}
			}

			AnimatedModelRenderer::~AnimatedModelRenderer()
			{

			}
			json::Value AnimatedModelRenderer::toJson(json::Value &meshes) const
			{
				json::Value ret;
				ret["type"] = "animatedmodelrenderer";
				ret["file"] = fileName;
				ret["castShadow"] = castShadow;
				ret["cullBackFaces"] = cullBackFaces;
				return ret;
			}

			void AnimatedModelRenderer::update(float elapsedTime, Scene& scene)
			{
				if (!model)
					return;
				modelInstance->update(elapsedTime * animationSpeed);

				if (callbackOnDone)
				{
					if (((vrlib::State*)modelInstance)->animations.size() == 0)
					{
						callbackOnDone();
						callbackOnDone = nullptr;
					}
				}

			}

			void AnimatedModelRenderer::drawDeferredPass()
			{
				if (!castShadow || !model || !visible)
					return;
				components::Transform* t = node->getComponent<Transform>();

				ModelRenderContext* context = dynamic_cast<ModelRenderContext*>(renderContextDeferred);
				context->renderShader->use(); //TODO: only call this once!
				//TODO: ewwww
				context->renderShader->setUniform(ModelRenderContext::RenderUniform::boneMatrices, ((vrlib::State*)modelInstance)->boneMatrices);


				if (!cullBackFaces)
					glDisable(GL_CULL_FACE);


				modelInstance->draw([this, t, &context](const glm::mat4 &modelMatrix)
				{
					context->renderShader->setUniform(ModelRenderContext::RenderUniform::modelMatrix, t->globalTransform);
					context->renderShader->setUniform(ModelRenderContext::RenderUniform::normalMatrix, glm::transpose(glm::inverse(glm::mat3(t->globalTransform))));
				},
					[this, &context](const vrlib::Material &material)
				{
					if (material.texture)
					{
						context->renderShader->setUniform(ModelRenderContext::RenderUniform::diffuseColor, glm::vec4(1,1,1,1));
						context->renderShader->setUniform(ModelRenderContext::RenderUniform::textureFactor, 1.0f);
						material.texture->bind();
						glActiveTexture(GL_TEXTURE1);
						if (material.normalmap)
							material.normalmap->bind();
						else
							context->defaultNormalMap->bind();
						
						if (material.specularmap)
						{
							glActiveTexture(GL_TEXTURE2);
							material.specularmap->bind();
						}
						context->renderShader->setUniform(ModelRenderContext::RenderUniform::shinyness, material.color.shinyness);
						glActiveTexture(GL_TEXTURE0);

					}
					else
					{
						context->renderShader->setUniform(ModelRenderContext::RenderUniform::textureFactor, 0.0f);
						context->renderShader->setUniform(ModelRenderContext::RenderUniform::diffuseColor, material.color.diffuse);
						
						glActiveTexture(GL_TEXTURE1);
						if (material.normalmap)
							material.normalmap->bind();
						else
							context->defaultNormalMap->bind();

						if (material.specularmap)
						{
							glActiveTexture(GL_TEXTURE2);
							material.specularmap->bind();
						}
						context->renderShader->setUniform(ModelRenderContext::RenderUniform::shinyness, material.color.shinyness);
						glActiveTexture(GL_TEXTURE0);
					}
					return true;
				});

				if (!cullBackFaces)
					glEnable(GL_CULL_FACE);

			}

			void AnimatedModelRenderer::drawForwardPass()
			{

			}
			void AnimatedModelRenderer::drawShadowMap()
			{
//				if(!castShadow)
//					return;
				components::Transform* t = node->getComponent<Transform>();

				ModelShadowRenderContext* context = dynamic_cast<ModelShadowRenderContext*>(renderContextShadow);
				context->renderShader->use(); //TODO: only call this once!
											  //TODO: ewwww
				context->renderShader->setUniform(ModelShadowRenderContext::RenderUniform::boneMatrices, ((vrlib::State*)modelInstance)->boneMatrices);

				modelInstance->draw([this, t, &context](const glm::mat4 &modelMatrix)
				{
					context->renderShader->setUniform(ModelShadowRenderContext::RenderUniform::modelMatrix, t->globalTransform);
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

			void AnimatedModelRenderer::resetAnimation()
			{
				((vrlib::State*)modelInstance)->stopAnimations();
				((vrlib::State*)modelInstance)->resetToInitial();
			}


			void AnimatedModelRenderer::ModelRenderContext::init()
			{
				renderShader = new vrlib::gl::Shader<RenderUniform>("data/vrlib/tien/shaders/AnimatedModelRenderer.deferred.vert", "data/vrlib/tien/shaders/AnimatedModelRenderer.deferred.frag");
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
				renderShader->registerUniform(RenderUniform::s_specularmap, "s_specularmap");
				renderShader->registerUniform(RenderUniform::diffuseColor, "diffuseColor");
				renderShader->registerUniform(RenderUniform::textureFactor, "textureFactor");
				renderShader->registerUniform(RenderUniform::boneMatrices, "boneMatrices");
				renderShader->registerUniform(RenderUniform::shinyness, "shinyness");
				renderShader->use();
				renderShader->setUniform(RenderUniform::s_texture, 0);
				renderShader->setUniform(RenderUniform::s_normalmap, 1);
				renderShader->setUniform(RenderUniform::s_specularmap, 2);

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

			void AnimatedModelRenderer::ModelShadowRenderContext::init()
			{
				renderShader = new vrlib::gl::Shader<RenderUniform>("data/vrlib/tien/shaders/AnimatedModelRenderer.shadow.vert", "data/vrlib/tien/shaders/AnimatedModelRenderer.shadow.frag");
				renderShader->bindAttributeLocation("a_position", 0);
				renderShader->bindAttributeLocation("a_boneIds", 5);
				renderShader->bindAttributeLocation("a_boneWeights", 6);
				renderShader->link();
				renderShader->registerUniform(RenderUniform::modelMatrix, "modelMatrix");
				renderShader->registerUniform(RenderUniform::projectionMatrix, "projectionMatrix");
				renderShader->registerUniform(RenderUniform::viewMatrix, "viewMatrix");
				renderShader->registerUniform(RenderUniform::boneMatrices, "boneMatrices");
				renderShader->registerUniform(RenderUniform::outputPosition, "outputPosition");
			}

			void AnimatedModelRenderer::ModelShadowRenderContext::frameSetup(const glm::mat4 & projectionMatrix, const glm::mat4 & viewMatrix)
			{
				renderShader->use();
				renderShader->setUniform(RenderUniform::projectionMatrix, projectionMatrix);
				renderShader->setUniform(RenderUniform::viewMatrix, viewMatrix);
			}
			void AnimatedModelRenderer::ModelShadowRenderContext::useCubemap(bool use)
			{
				renderShader->use();
				renderShader->setUniform(RenderUniform::outputPosition, use);
			}

			void AnimatedModelRenderer::buildEditor(EditorBuilder * builder)
			{
				builder->addTitle("Model Renderer");

				builder->beginGroup("Filename", false);
				EditorBuilder::TextComponent* filenameBox = builder->addTextBox(fileName, [this](const std::string &) {});
				builder->addBrowseButton(EditorBuilder::BrowseType::Model, [this, filenameBox](const std::string &file)
				{
					fileName = file;
					filenameBox->setText(fileName);
					if (cache.find(fileName) == cache.end())
						cache[fileName] = vrlib::Model::getModel<vrlib::gl::VertexP3N2B2T2T2>(fileName);
					model = cache[fileName];
					modelInstance = model->getInstance();
				});
				builder->endGroup();

				builder->beginGroup("Casts Shadows");
				builder->addCheckbox(castShadow, [this](bool newValue) {castShadow = newValue; });
				builder->endGroup();

				builder->beginGroup("Cull backfaces");
				builder->addCheckbox(cullBackFaces, [this](bool newValue) {	cullBackFaces = newValue; });
				builder->endGroup();
				if (!model)
					return;
				builder->beginGroup("Animation");
				if (model)
				{
					std::vector<std::string> animations = model->getAnimationNames();
					builder->addComboBox("", animations, [this](const std::string &newValue)
					{
						if (!((vrlib::State*)modelInstance)->animations.empty())
							((vrlib::State*)modelInstance)->stopAnimation(((vrlib::State*)modelInstance)->animations[0]->animation->name);
						playAnimation(newValue, true); //TODO: this doesn't work yet?
					});
				}
				builder->endGroup();


				builder->addTitle("Materials");
				builder->beginGroup("Has alpha materials");
				if(model)
					builder->addCheckbox(model->hasAlphaMaterial(), [](bool newValue) {});
				builder->endGroup();
				int index = 0; //TODO: material name?
				for (auto m : model->getMaterials())
				{
					builder->addTitle("Material " + std::to_string(index));

					builder->beginGroup("Ambient", true);
					builder->addTextBox("#AABBCC", [](const std::string &newText) {}); //TODO: color picker
					builder->endGroup();
					builder->beginGroup("Diffuse", true);
					builder->addTextBox("#AABBCC", [](const std::string &newText) {}); //TODO: color picker
					builder->endGroup();
					builder->beginGroup("Specular", true);
					builder->addTextBox("#AABBCC", [](const std::string &newText) {}); //TODO: color picker
					builder->endGroup();

					builder->beginGroup("Texture", false);
					builder->addTextBox((m->texture && m->texture->image) ? m->texture->image->fileName : "", [](const std::string &newFile) {});
					builder->addBrowseButton(EditorBuilder::BrowseType::Texture, [](const std::string &onClick)
					{

					});
					builder->endGroup();

					builder->beginGroup("Normalmap", false);
					builder->addTextBox((m->normalmap && m->normalmap->image) ? m->normalmap->image->fileName : "", [](const std::string &newFile) {});
					builder->addBrowseButton(EditorBuilder::BrowseType::Texture, [](const std::string &onClick)
					{

					});
					builder->endGroup();

					builder->beginGroup("Specularmap");
					builder->addTextBox((m->normalmap && m->normalmap->image) ? m->normalmap->image->fileName : "", [](const std::string &newFile) {});
					builder->addBrowseButton(EditorBuilder::BrowseType::Texture, [](const std::string &onClick)
					{

					});
					builder->addTextBox("1.0", [](const std::string &newValue) {});
					builder->endGroup();


					builder->beginGroup("Glow");
					builder->addTextBox("", [](const std::string &newFile) {});
					builder->addBrowseButton(EditorBuilder::BrowseType::Texture, [](const std::string &onClick)
					{

					});
					builder->addTextBox("1.0", [](const std::string &newValue) {});
					builder->endGroup();

					index++;
				}

				builder->beginGroup("");

				builder->endGroup();

			}

		}
	}
}