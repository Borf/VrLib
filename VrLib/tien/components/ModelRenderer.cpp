#include "ModelRenderer.h"
#include <VrLib/Model.h>
#include <VrLib/Texture.h>
#include <VrLib/gl/Vertex.h>
#include <VrLib/json.h>
#include <VrLib/Image.h>
#include "Transform.h"
#include "../Node.h"


namespace vrlib
{
	namespace tien
	{
		namespace components
		{
			std::map<std::string, vrlib::Model*> ModelRenderer::cache;

/*			ModelRenderer::ModelRenderer(const std::string &fileName) : fileName(fileName)
			{
				if (cache.find(fileName) == cache.end())
					cache[fileName] = vrlib::Model::getModel<vrlib::gl::VertexP3N2B2T2T2>(fileName);
				model = cache[fileName];
				renderContextDeferred = ModelDeferredRenderContext::getInstance();
				renderContextShadow = ModelShadowRenderContext::getInstance();
				castShadow = true;
			}*/

			ModelRenderer::ModelRenderer(const vrlib::json::Value &json)
			{
				if (json.isString())
					fileName = json;
				else
					fileName = json["file"];
				if (cache.find(fileName) == cache.end())
					cache[fileName] = vrlib::Model::getModel<vrlib::gl::VertexP3N2B2T2T2>(fileName);
				model = cache[fileName];
				renderContextDeferred = ModelDeferredRenderContext::getInstance();
				renderContextShadow = ModelShadowRenderContext::getInstance();
				renderContextForward = ModelForwardRenderContext::getInstance();
				if (json.isObject())
				{
					castShadow = json["castShadow"];
					cullBackFaces = json["cullBackFaces"];
				}
				else
				{
					castShadow = true;
					cullBackFaces = true;
				}
			}


			json::Value ModelRenderer::toJson(json::Value &meshes) const
			{
				json::Value ret;
				ret["type"] = "modelrenderer";
				ret["file"] = fileName;
				ret["castShadow"] = castShadow;
				ret["cullBackFaces"] = cullBackFaces;
				return ret;
			}

			void ModelRenderer::buildEditor(EditorBuilder * builder)
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
				});
				builder->endGroup();

				builder->beginGroup("Casts Shadows");
				builder->addCheckbox(castShadow, [this](bool newValue) {castShadow = newValue; });
				builder->endGroup();

				builder->beginGroup("Cull backfaces");
				builder->addCheckbox(cullBackFaces, [this](bool newValue) {	cullBackFaces = newValue; });
				builder->endGroup();

				builder->addTitle("Materials");
				builder->beginGroup("Has alpha materials");
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
					builder->addTextBox((m->specularmap && m->specularmap->image) ? m->specularmap->image->fileName : "", [](const std::string &newFile) {});
					builder->addBrowseButton(EditorBuilder::BrowseType::Texture, [](const std::string &onClick)
					{

					});
					builder->addTextBox(std::to_string(m->color.shinyness), [](const std::string &newValue) {});
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

			ModelRenderer::~ModelRenderer()
			{

			}

			void ModelRenderer::drawDeferredPass()
			{
				if (!model)
					return;
				components::Transform* t = node->getComponent<Transform>();

				ModelDeferredRenderContext* context = dynamic_cast<ModelDeferredRenderContext*>(renderContextDeferred);
				context->renderShader->use(); //TODO: only call this once!
				
				if (!cullBackFaces)
					glDisable(GL_CULL_FACE);

				model->draw([this, t, &context](const glm::mat4 &modelMatrix)
				{
					context->renderShader->setUniform(ModelDeferredRenderContext::RenderUniform::modelMatrix, t->globalTransform * modelMatrix);
					context->renderShader->setUniform(ModelDeferredRenderContext::RenderUniform::normalMatrix, glm::mat3(glm::transpose(glm::inverse(t->globalTransform * modelMatrix))));
				},
					[this, &context](const vrlib::Material &material)
				{
					if (material.texture->usesAlphaChannel)
						return false;
					if (material.texture)
					{
						context->renderShader->setUniform(ModelDeferredRenderContext::RenderUniform::textureFactor, 1.0f);
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
					
						context->renderShader->setUniform(ModelDeferredRenderContext::RenderUniform::shinyness, material.color.shinyness);
						glActiveTexture(GL_TEXTURE0);
					}
					else
					{
						context->renderShader->setUniform(ModelDeferredRenderContext::RenderUniform::textureFactor, 0.0f);
						context->renderShader->setUniform(ModelDeferredRenderContext::RenderUniform::diffuseColor, material.color.diffuse);
						glActiveTexture(GL_TEXTURE1);
						context->defaultNormalMap->bind();
						glActiveTexture(GL_TEXTURE0);
					}
					return true;
				});

				if (!cullBackFaces)
					glEnable(GL_CULL_FACE);
			}

			void ModelRenderer::drawForwardPass()
			{
				if (!model)
					return;
				ModelForwardRenderContext* context = dynamic_cast<ModelForwardRenderContext*>(renderContextForward);
				if (!context)
					return;
				context->renderShader->use(); //TODO: only call this once!

				components::Transform* t = node->getComponent<Transform>();

				if (!cullBackFaces)
					glDisable(GL_CULL_FACE);

				model->draw([this, t, &context](const glm::mat4 &modelMatrix)
				{
					context->renderShader->setUniform(ModelForwardRenderContext::RenderUniform::modelMatrix, t->globalTransform * modelMatrix);
					context->renderShader->setUniform(ModelForwardRenderContext::RenderUniform::normalMatrix, glm::mat3(glm::transpose(glm::inverse(t->globalTransform * modelMatrix))));
				},
					[this, &context](const vrlib::Material &material)
				{
					if (!material.texture->usesAlphaChannel)
						return false;
					if (material.texture)
					{
						context->renderShader->setUniform(ModelForwardRenderContext::RenderUniform::textureFactor, 1.0f);
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

						context->renderShader->setUniform(ModelForwardRenderContext::RenderUniform::shinyness, material.color.shinyness);
						glActiveTexture(GL_TEXTURE0);
					}
					else
					{
						context->renderShader->setUniform(ModelForwardRenderContext::RenderUniform::textureFactor, 0.0f);
						context->renderShader->setUniform(ModelForwardRenderContext::RenderUniform::diffuseColor, material.color.diffuse);
						glActiveTexture(GL_TEXTURE1);
						context->defaultNormalMap->bind();
						glActiveTexture(GL_TEXTURE0);
					}
					return true;
				});

				if (!cullBackFaces)
					glEnable(GL_CULL_FACE);
			}



			void ModelRenderer::drawShadowMap()
			{
				if (!castShadow || !model)
					return;
				if (!cullBackFaces)
					glDisable(GL_CULL_FACE);
				components::Transform* t = node->getComponent<Transform>();
				ModelShadowRenderContext* context = dynamic_cast<ModelShadowRenderContext*>(renderContextShadow);
				context->renderShader->use(); //TODO: only call this once!
				model->draw([this, t, &context](const glm::mat4 &modelMatrix)
				{
					context->renderShader->setUniform(ModelShadowRenderContext::RenderUniform::modelMatrix, t->globalTransform * modelMatrix);
				});
				if (!cullBackFaces)
					glEnable(GL_CULL_FACE);

			}




			void ModelRenderer::ModelDeferredRenderContext::init()
			{
				renderShader = new vrlib::gl::Shader<RenderUniform>("data/vrlib/tien/shaders/ModelRenderer.deferred.vert", "data/vrlib/tien/shaders/ModelRenderer.deferred.frag");
				renderShader->bindAttributeLocation("a_position", 0);
				renderShader->bindAttributeLocation("a_normal", 1);
				renderShader->bindAttributeLocation("a_bitangent", 2);
				renderShader->bindAttributeLocation("a_tangent", 3);
				renderShader->bindAttributeLocation("a_texture", 4);
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
				renderShader->registerUniform(RenderUniform::shinyness, "shinyness");
				renderShader->use();
				renderShader->setUniform(RenderUniform::s_texture, 0);
				renderShader->setUniform(RenderUniform::s_normalmap, 1);
				renderShader->setUniform(RenderUniform::s_specularmap, 2);

				defaultNormalMap = vrlib::Texture::loadCached("data/vrlib/tien/textures/defaultnormalmap.png");

			}

			void ModelRenderer::ModelDeferredRenderContext::frameSetup(const glm::mat4 & projectionMatrix, const glm::mat4 & viewMatrix)
			{
				renderShader->use();
				renderShader->setUniform(RenderUniform::projectionMatrix, projectionMatrix);
				renderShader->setUniform(RenderUniform::viewMatrix, viewMatrix);
				renderShader->setUniform(RenderUniform::diffuseColor, glm::vec4(1, 1, 1, 1));
				renderShader->setUniform(RenderUniform::textureFactor, 1.0f);
			}







			void ModelRenderer::ModelForwardRenderContext::init()
			{
				renderShader = new vrlib::gl::Shader<RenderUniform>("data/vrlib/tien/shaders/ModelRenderer.forward.vert", "data/vrlib/tien/shaders/ModelRenderer.forward.frag");
				renderShader->bindAttributeLocation("a_position", 0);
				renderShader->bindAttributeLocation("a_normal", 1);
				renderShader->bindAttributeLocation("a_bitangent", 2);
				renderShader->bindAttributeLocation("a_tangent", 3);
				renderShader->bindAttributeLocation("a_texture", 4);
				renderShader->link();
				renderShader->bindFragLocation("fragColor", 0);
				renderShader->registerUniform(RenderUniform::modelMatrix, "modelMatrix");
				renderShader->registerUniform(RenderUniform::projectionMatrix, "projectionMatrix");
				renderShader->registerUniform(RenderUniform::viewMatrix, "viewMatrix");
				renderShader->registerUniform(RenderUniform::normalMatrix, "normalMatrix");
				renderShader->registerUniform(RenderUniform::s_texture, "s_texture");
				renderShader->registerUniform(RenderUniform::s_normalmap, "s_normalmap");
				renderShader->registerUniform(RenderUniform::s_specularmap, "s_specularmap");
				renderShader->registerUniform(RenderUniform::diffuseColor, "diffuseColor");
				renderShader->registerUniform(RenderUniform::textureFactor, "textureFactor");
				renderShader->registerUniform(RenderUniform::shinyness, "shinyness");
				renderShader->use();
				renderShader->setUniform(RenderUniform::s_texture, 0);
				renderShader->setUniform(RenderUniform::s_normalmap, 1);
				renderShader->setUniform(RenderUniform::s_specularmap, 2);

				defaultNormalMap = vrlib::Texture::loadCached("data/vrlib/tien/textures/defaultnormalmap.png");

			}

			void ModelRenderer::ModelForwardRenderContext::frameSetup(const glm::mat4 & projectionMatrix, const glm::mat4 & viewMatrix)
			{
				renderShader->use();
				renderShader->setUniform(RenderUniform::projectionMatrix, projectionMatrix);
				renderShader->setUniform(RenderUniform::viewMatrix, viewMatrix);
				renderShader->setUniform(RenderUniform::diffuseColor, glm::vec4(1, 1, 1, 1));
				renderShader->setUniform(RenderUniform::textureFactor, 1.0f);
			}










			void ModelRenderer::ModelShadowRenderContext::init()
			{
				renderShader = new vrlib::gl::Shader<RenderUniform>("data/vrlib/tien/shaders/ModelRenderer.shadow.vert", "data/vrlib/tien/shaders/ModelRenderer.shadow.frag");
				renderShader->bindAttributeLocation("a_position", 0);
				renderShader->bindAttributeLocation("a_normal", 1);
				renderShader->bindAttributeLocation("a_bitangent", 2);
				renderShader->bindAttributeLocation("a_tangent", 3);
				renderShader->bindAttributeLocation("a_texture", 4);
				renderShader->link();
				renderShader->registerUniform(RenderUniform::modelMatrix, "modelMatrix");
				renderShader->registerUniform(RenderUniform::projectionMatrix, "projectionMatrix");
				renderShader->registerUniform(RenderUniform::viewMatrix, "viewMatrix");
				renderShader->registerUniform(RenderUniform::outputPosition, "outputPosition");				
				renderShader->use();
			}

			void ModelRenderer::ModelShadowRenderContext::frameSetup(const glm::mat4 & projectionMatrix, const glm::mat4 & viewMatrix)
			{
				renderShader->use();
				renderShader->setUniform(RenderUniform::projectionMatrix, projectionMatrix);
				renderShader->setUniform(RenderUniform::viewMatrix, viewMatrix);
			}
			void ModelRenderer::ModelShadowRenderContext::useCubemap(bool use)
			{
				renderShader->use();
				renderShader->setUniform(RenderUniform::outputPosition, use);
			}
		}

	}
}