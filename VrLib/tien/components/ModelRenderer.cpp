#include "ModelRenderer.h"
#include <VrLib/Model.h>
#include <VrLib/Texture.h>
#include <VrLib/gl/Vertex.h>
#include <VrLib/json.hpp>
#include <VrLib/Image.h>
#include "Transform.h"
#include "../Node.h"
#include "../Renderer.h"
#include <VrLib\Log.h>

namespace vrlib
{
	namespace tien
	{
		namespace components
		{
			std::map<std::string, vrlib::Model*> ModelRenderer::cache;

			ModelRenderer::ModelRenderer(const json &data)
			{
				if (data.is_string())
					fileName = data.get<std::string>();
				else
					fileName = data["file"].get<std::string>();
				if (cache.find(fileName) == cache.end())
					cache[fileName] = vrlib::Model::getModel<vrlib::gl::VertexP3N2B2T2T2>(fileName);
				model = cache[fileName];
				renderContextDeferred = ModelDeferredRenderContext::getInstance();
				renderContextShadow = ModelShadowRenderContext::getInstance();
				if (data.find("forwardShader") == data.end())
					renderContextForward = ModelForwardRenderContext::getInstance("");
				else
					renderContextForward = ModelForwardRenderContext::getInstance(data["forwardShader"]);

				if (data.is_object())
				{
					castShadow = data["castShadow"];
					cullBackFaces = data["cullBackFaces"];
				}
				else
				{
					castShadow = true;
					cullBackFaces = true;
				}
				hasForward = false;
				if (model) {
					for (auto m : model->getMaterials()) {
						materialOverrides[m] = *m;
					}
				} else {
#ifdef _DEBUG
					vrlib::logger << "model was null!" << data["file"] << vrlib::Log::newline;
#endif // _DEBUG
					return;
				}

				prevModel = model;

				if (data.find("materialoverrides") != data.end() && model)
				{
					std::vector<Material*> orig = model->getMaterials();
					int i = 0;
					for (const json &v : data["materialoverrides"])
					{
						Material* overrideMaterial = &materialOverrides[orig[i]];
						if (v.find("texture") != v.end())
							overrideMaterial->texture = vrlib::Texture::loadCached(v["texture"]);
						if (v.find("normalmap") != v.end())
							overrideMaterial->normalmap = vrlib::Texture::loadCached(v["normalmap"]);
						if (v.find("specularmap") != v.end())
							overrideMaterial->specularmap = vrlib::Texture::loadCached(v["specularmap"]);

						i++;
					}
				}

			}


			json ModelRenderer::toJson(json &meshes) const
			{
				json ret;
				ret["type"] = "modelrenderer";
				ret["file"] = fileName;
				ret["castShadow"] = castShadow;
				ret["cullBackFaces"] = cullBackFaces;
				if (!model)
					return ret;
				for (vrlib::Material* m : model->getMaterials()) //TODO: getMaterials might not always return same order
				{
					const Material &overrideMaterial = materialOverrides.find(m)->second;
					json v;					
					if (m->texture != overrideMaterial.texture && overrideMaterial.texture)
						v["texture"] = overrideMaterial.texture->image->fileName;
					if (m->normalmap != overrideMaterial.normalmap && overrideMaterial.normalmap)
						v["normalmap"] = overrideMaterial.normalmap->image->fileName;
					if (m->specularmap != overrideMaterial.specularmap && overrideMaterial.specularmap)
						v["specularmap"] = overrideMaterial.specularmap->image->fileName;
					ret["materialoverrides"].push_back(v);
				}
				return ret;
			}

			Material* ModelRenderer::getCurrentMaterial()
			{
				return model->getMaterials()[0];
			}

			void ModelRenderer::buildEditor(EditorBuilder * builder, bool folded)
			{
				builder->addTitle("Model Renderer");
				if (folded)
					return;
				builder->beginGroup("Filename", false);
				builder->addModelBox(fileName, [this](const std::string &file) {
					fileName = file;
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

				if (!model)
					return;

				builder->addTitle("Materials");
				builder->beginGroup("Has alpha materials");
				builder->addCheckbox(model->hasAlphaMaterial(), [](bool newValue) {});
				builder->endGroup();

				int index = 0; //TODO: material name?
				for (auto materialLookup : materialOverrides)
				{
					Material* m = &materialLookup.second;

					builder->addTitle("Material " + std::to_string(index));

					builder->beginGroup("Ambient", true);
					builder->addColorBox(m->color.ambient, [m](const glm::vec4 &newColor) { m->color.ambient = newColor;  }); //TODO: color picker
					builder->endGroup();
					builder->beginGroup("Diffuse", true);
					builder->addColorBox(m->color.diffuse, [m](const glm::vec4 &newColor) { m->color.diffuse = newColor;  }); //TODO: color picker
					builder->endGroup();
					builder->beginGroup("Specular", true);
					builder->addColorBox(glm::vec4(m->color.specular,1), [m](const glm::vec4 &newColor) { m->color.specular = glm::vec3(newColor);  }); //TODO: color picker
					builder->endGroup();

					builder->beginGroup("Texture", false);
					auto textureBox = builder->addTextureBox((m->texture && m->texture->image) ? m->texture->image->fileName : "", [this, materialLookup](const std::string &newFile)	{
						materialOverrides[materialLookup.first].texture = vrlib::Texture::loadCached(newFile);
					});
					builder->addSmallButton("x", [this, materialLookup, textureBox]()
					{
						textureBox->setText("");
						materialOverrides[materialLookup.first].texture = nullptr;
					});
					builder->endGroup();

					builder->beginGroup("Normalmap", false);
					auto normalBox = builder->addTextureBox((m->normalmap && m->normalmap->image) ? m->normalmap->image->fileName : "", [this, materialLookup](const std::string &newFile) {
						materialOverrides[materialLookup.first].normalmap = vrlib::Texture::loadCached(newFile);
					});
					builder->addSmallButton("x", [this, materialLookup, normalBox]()
					{
						normalBox->setText("");
						materialOverrides[materialLookup.first].normalmap = nullptr;
					});
					builder->endGroup();

					builder->beginGroup("Specularmap", false);
					auto specBox = builder->addTextureBox((m->specularmap && m->specularmap->image) ? m->specularmap->image->fileName : "", [this, materialLookup](const std::string &newFile) {
						materialOverrides[materialLookup.first].specularmap = vrlib::Texture::loadCached(newFile);
					});
					builder->addSmallButton("x", [this, materialLookup, specBox]()
					{
						specBox->setText("");
						materialOverrides[materialLookup.first].specularmap = nullptr;
					});
					builder->endGroup();

					builder->beginGroup("Shinyness");
					builder->addFloatBox(m->color.shinyness, 0, 300, [m](float newValue) { m->color.shinyness = newValue; });
					builder->endGroup();

					index++;
				}

				builder->beginGroup("");

				builder->endGroup();

			}

			ModelRenderer::~ModelRenderer()
			{

			}
			//TODO: deferred and forward code looks quite the same.... look for some code reuse
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
					glm::mat4 mat(t->globalTransform * modelMatrix);
					context->renderShader->setUniform(ModelDeferredRenderContext::RenderUniform::modelMatrix, mat);
					context->renderShader->setUniform(ModelDeferredRenderContext::RenderUniform::normalMatrix, glm::transpose(glm::inverse(glm::mat3(mat))));
				},
					[this, &context](const vrlib::Material &m)
				{
					Material& material = materialOverrides[const_cast<vrlib::Material*>(&m)];

					if ((material.texture && material.texture->usesAlphaChannel) || material.color.diffuse.a < 0.999f)
					{
						hasForward = true;
						return false;
					}
					if (material.texture)
					{
						context->renderShader->setUniform(ModelDeferredRenderContext::RenderUniform::textureFactor, 1.0f);
						material.texture->bind();
					}
					else
					{
						context->renderShader->setUniform(ModelDeferredRenderContext::RenderUniform::textureFactor, 0.0f);
						context->renderShader->setUniform(ModelDeferredRenderContext::RenderUniform::diffuseColor, material.color.diffuse);
					}
					glActiveTexture(GL_TEXTURE1);
					if (material.normalmap)
					{
						context->renderShader->setUniform(ModelDeferredRenderContext::RenderUniform::hasNormalMap, 1.0f);
						material.normalmap->bind();
					}
					else
					{
						context->renderShader->setUniform(ModelDeferredRenderContext::RenderUniform::hasNormalMap, 0.0f);
						context->defaultNormalMap->bind();
					}

					glActiveTexture(GL_TEXTURE2);
					if (material.specularmap)
						material.specularmap->bind();
					else
						context->white->bind();

					context->renderShader->setUniform(ModelDeferredRenderContext::RenderUniform::shinyness, material.color.shinyness);
					glActiveTexture(GL_TEXTURE0);


					Renderer::drawCalls++;
					return true;
				});

				if (!cullBackFaces)
					glEnable(GL_CULL_FACE);
			}

			void ModelRenderer::drawForwardPass()
			{
				if (!model || !hasForward)
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
					[this, &context](const vrlib::Material &m)
				{
					Material& material = materialOverrides[const_cast<vrlib::Material*>(&m)];

					if (material.color.diffuse.a >= 0.999f && (!material.texture || !material.texture->usesAlphaChannel))
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
					Renderer::drawCalls++;
					return true;
				});

				if (!cullBackFaces)
					glEnable(GL_CULL_FACE);
			}



			void ModelRenderer::drawShadowMap()
			{
				if (!castShadow || !model || !visible)
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


			void ModelRenderer::update(float elapsedTime, Scene& scene)
			{
				if (model != prevModel)
				{
					materialOverrides.clear();
					for (auto m : model->getMaterials())
						materialOverrides[m] = *m;
					prevModel = model;
				}
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
				renderShader->registerUniform(RenderUniform::hasNormalMap, "hasNormalMap");
				renderShader->use();
				renderShader->setUniform(RenderUniform::s_texture, 0);
				renderShader->setUniform(RenderUniform::s_normalmap, 1);
				renderShader->setUniform(RenderUniform::s_specularmap, 2);

				defaultNormalMap = vrlib::Texture::loadCached("data/vrlib/tien/textures/defaultnormalmap.png");
				white = vrlib::Texture::loadCached("data/vrlib/tien/textures/white.png");
			}

			void ModelRenderer::ModelDeferredRenderContext::frameSetup(const glm::mat4 & projectionMatrix, const glm::mat4 & viewMatrix)
			{
				renderShader->use();
				renderShader->setUniform(RenderUniform::projectionMatrix, projectionMatrix);
				renderShader->setUniform(RenderUniform::viewMatrix, viewMatrix);
				renderShader->setUniform(RenderUniform::diffuseColor, glm::vec4(1, 1, 1, 1));
				renderShader->setUniform(RenderUniform::textureFactor, 1.0f);
			}






			//ModelRenderer::ModelForwardRenderContext::ModelForwardRenderContext()
			ModelRenderer::ModelForwardRenderContext::ModelForwardRenderContext(const std::string & shader)
			{
				shaderFile = shader;
				if (shaderFile == "")
					shaderFile = "data/vrlib/tien/shaders/ModelRenderer.forward";

			}

			void ModelRenderer::ModelForwardRenderContext::init()
			{
				renderShader = new vrlib::gl::Shader<RenderUniform>(shaderFile + ".vert", shaderFile + ".frag");
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