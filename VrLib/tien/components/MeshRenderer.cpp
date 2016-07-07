#include "MeshRenderer.h"
#include <VrLib/Model.h>
#include <VrLib/Texture.h>
#include <VrLib/gl/Vertex.h>
#include "Transform.h"
#include "../Node.h"


namespace vrlib
{
	namespace tien
	{
		namespace components
		{
			MeshRenderer::MeshRenderer(Mesh* mesh)
			{
				this->mesh = mesh;
				vao = nullptr;
				if (mesh)
					updateMesh();
				castShadow = true;
				renderContext = ModelRenderContext::getInstance();
				renderContextShadow = ModelRenderShadowContext::getInstance();
			}

			MeshRenderer::~MeshRenderer()
			{

			}


			void MeshRenderer::updateMesh()
			{
				vbo.setData(mesh->vertices.size(), &mesh->vertices[0], GL_STATIC_DRAW);
				vio.setData(mesh->indices.size(), &mesh->indices[0], GL_STATIC_DRAW);

				if (!vao)
					vao = new gl::VAO<gl::VertexP3N2B2T2T2>(&vbo);
				vao->bind();
				vio.bind();
				vao->unBind();
			}


			void MeshRenderer::draw()
			{
				components::Transform* t = node->getComponent<Transform>();

				ModelRenderContext* context = dynamic_cast<ModelRenderContext*>(renderContext);
				context->renderShader->use(); //TODO: only call this once!

				context->renderShader->setUniform(ModelRenderContext::RenderUniform::modelMatrix, t->globalTransform);
				context->renderShader->setUniform(ModelRenderContext::RenderUniform::normalMatrix, glm::transpose(glm::inverse(glm::mat3(t->globalTransform))));
				if (mesh->material.texture)
				{
					context->renderShader->setUniform(ModelRenderContext::RenderUniform::textureFactor, 1.0f);
					mesh->material.texture->bind();
					glActiveTexture(GL_TEXTURE1);
					if (mesh->material.normalmap)
						mesh->material.normalmap->bind();
					else
						context->defaultNormalMap->bind();
					glActiveTexture(GL_TEXTURE0);

				}
				else
				{
					context->renderShader->setUniform(ModelRenderContext::RenderUniform::textureFactor, 0.0f);
					context->renderShader->setUniform(ModelRenderContext::RenderUniform::diffuseColor, mesh->material.color.diffuse);
				}


				if (vao)
				{
					vao->bind();
					glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
					vao->unBind();
				}
			}


			void MeshRenderer::drawShadowMap()
			{
				if (!castShadow)
					return;
				components::Transform* t = node->getComponent<Transform>();
				ModelRenderShadowContext* context = dynamic_cast<ModelRenderShadowContext*>(renderContextShadow);
				context->renderShader->use(); //TODO: only call this once!
				context->renderShader->setUniform(ModelRenderShadowContext::RenderUniform::modelMatrix, t->globalTransform);


			}


			void MeshRenderer::ModelRenderContext::init()
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

			void MeshRenderer::ModelRenderContext::frameSetup(const glm::mat4 & projectionMatrix, const glm::mat4 & viewMatrix)
			{
				renderShader->use();
				renderShader->setUniform(RenderUniform::projectionMatrix, projectionMatrix);
				renderShader->setUniform(RenderUniform::viewMatrix, viewMatrix);
				renderShader->setUniform(RenderUniform::diffuseColor, glm::vec4(1, 1, 1, 1));
				renderShader->setUniform(RenderUniform::textureFactor, 1.0f);
			}


			void MeshRenderer::ModelRenderShadowContext::init()
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

			void MeshRenderer::ModelRenderShadowContext::frameSetup(const glm::mat4 & projectionMatrix, const glm::mat4 & viewMatrix)
			{
				renderShader->use();
				renderShader->setUniform(RenderUniform::projectionMatrix, projectionMatrix);
				renderShader->setUniform(RenderUniform::viewMatrix, viewMatrix);
			}
			void MeshRenderer::ModelRenderShadowContext::useCubemap(bool use)
			{
				renderShader->use();
				renderShader->setUniform(RenderUniform::outputPosition, use);
			}
		}

	}
}