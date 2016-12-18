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
				renderContext = ModelRenderContext::getInstance();
				renderContextShadow = ModelRenderShadowContext::getInstance();
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
				renderContext = ModelRenderContext::getInstance();
				renderContextShadow = ModelRenderShadowContext::getInstance();
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
				builder->addCheckbox(true, [](bool newValue) {});
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


					builder->beginGroup("Glow", false);
					builder->endGroup();

					index++;
				}

				builder->beginGroup("");

				builder->endGroup();

			}

			ModelRenderer::~ModelRenderer()
			{

			}

			void ModelRenderer::draw()
			{
				if (!model)
					return;
				components::Transform* t = node->getComponent<Transform>();

				ModelRenderContext* context = dynamic_cast<ModelRenderContext*>(renderContext);
				context->renderShader->use(); //TODO: only call this once!
				
				if (!cullBackFaces)
					glDisable(GL_CULL_FACE);

				model->draw([this, t, &context](const glm::mat4 &modelMatrix)
				{
					context->renderShader->setUniform(ModelRenderContext::RenderUniform::modelMatrix, t->globalTransform * modelMatrix);
					context->renderShader->setUniform(ModelRenderContext::RenderUniform::normalMatrix, glm::transpose(glm::inverse(glm::mat3(t->globalTransform * modelMatrix))));
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
						glActiveTexture(GL_TEXTURE1);
						context->defaultNormalMap->bind();
						glActiveTexture(GL_TEXTURE0);
					}
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
				ModelRenderShadowContext* context = dynamic_cast<ModelRenderShadowContext*>(renderContextShadow);
				context->renderShader->use(); //TODO: only call this once!
				model->draw([this, t, &context](const glm::mat4 &modelMatrix)
				{
					context->renderShader->setUniform(ModelRenderShadowContext::RenderUniform::modelMatrix, t->globalTransform * modelMatrix);
				},
				[this, &context](const vrlib::Material &material)		{	});
				if (!cullBackFaces)
					glEnable(GL_CULL_FACE);

			}




			void ModelRenderer::ModelRenderContext::init()
			{
				renderShader = new vrlib::gl::Shader<RenderUniform>("data/vrlib/tien/shaders/default.vert", "data/vrlib/tien/shaders/default.frag");
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
				renderShader->registerUniform(RenderUniform::diffuseColor, "diffuseColor");
				renderShader->registerUniform(RenderUniform::textureFactor, "textureFactor");
				renderShader->use();
				renderShader->setUniform(RenderUniform::s_texture, 0);
				renderShader->setUniform(RenderUniform::s_normalmap, 1);

				defaultNormalMap = vrlib::Texture::loadCached("data/vrlib/tien/textures/defaultnormalmap.png");

			}

			void ModelRenderer::ModelRenderContext::frameSetup(const glm::mat4 & projectionMatrix, const glm::mat4 & viewMatrix)
			{
				renderShader->use();
				renderShader->setUniform(RenderUniform::projectionMatrix, projectionMatrix);
				renderShader->setUniform(RenderUniform::viewMatrix, viewMatrix);
				renderShader->setUniform(RenderUniform::diffuseColor, glm::vec4(1, 1, 1, 1));
				renderShader->setUniform(RenderUniform::textureFactor, 1.0f);
			}


			void ModelRenderer::ModelRenderShadowContext::init()
			{
				renderShader = new vrlib::gl::Shader<RenderUniform>("data/vrlib/tien/shaders/defaultShadow.vert", "data/vrlib/tien/shaders/defaultShadow.frag");
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

			void ModelRenderer::ModelRenderShadowContext::frameSetup(const glm::mat4 & projectionMatrix, const glm::mat4 & viewMatrix)
			{
				renderShader->use();
				renderShader->setUniform(RenderUniform::projectionMatrix, projectionMatrix);
				renderShader->setUniform(RenderUniform::viewMatrix, viewMatrix);
			}
			void ModelRenderer::ModelRenderShadowContext::useCubemap(bool use)
			{
				renderShader->use();
				renderShader->setUniform(RenderUniform::outputPosition, use);
			}
		}

	}
}