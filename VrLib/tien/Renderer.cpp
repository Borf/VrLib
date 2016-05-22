#include "Renderer.h"

#include "components/ModelRenderer.h"
#include "components/Transform.h"
#include "components/Camera.h"

#include <VrLib/gl/FBO.h>
#include <VrLib/Model.h>
#include <VrLib/Texture.h>


namespace vrlib
{
	namespace tien
	{

		Renderer::Renderer() : Node("Root", nullptr)
		{
			cameraNode = nullptr;
		}


		void Renderer::setTreeDirty()
		{
			treeDirty = true;
		}


		void Renderer::updateRenderables()
		{
			renderables.clear();
			fortree([this](Node* n)
			{
				if (n->getComponent<components::ModelRenderer>())
					renderables.push_back(n);
			});
		}



		void Renderer::init()
		{
			renderShader = new vrlib::gl::Shader<RenderUniform>("data/DeferredRenderer/shaders/default.vert", "data/DeferredRenderer/shaders/default.frag");
			renderShader->bindAttributeLocation("a_position", 0);
			renderShader->bindAttributeLocation("a_normal", 1);
			renderShader->bindAttributeLocation("a_texcoord", 2);
			renderShader->link();
			renderShader->bindFragLocation("fragColor", 0);
			renderShader->bindFragLocation("fragNormal", 1);
			renderShader->bindFragLocation("fragPosition", 2);
			//shader->bindFragLocation("fragColor", 0);
			renderShader->registerUniform(RenderUniform::modelMatrix, "modelMatrix");
			renderShader->registerUniform(RenderUniform::projectionMatrix, "projectionMatrix");
			renderShader->registerUniform(RenderUniform::viewMatrix, "viewMatrix");
			renderShader->registerUniform(RenderUniform::s_texture, "s_texture");
			renderShader->registerUniform(RenderUniform::diffuseColor, "diffuseColor");
			renderShader->registerUniform(RenderUniform::textureFactor, "textureFactor");
			renderShader->use();
			renderShader->setUniform(RenderUniform::s_texture, 0);


			postLightingShader = new vrlib::gl::Shader<PostLightingUniform>("data/DeferredRenderer/shaders/postprocess.vert", "data/DeferredRenderer/shaders/postprocess.frag");
			postLightingShader->bindAttributeLocation("a_position", 0);
			postLightingShader->link();
			postLightingShader->bindFragLocation("fragColor", 0);
			postLightingShader->registerUniform(PostLightingUniform::s_color, "s_color");
			postLightingShader->registerUniform(PostLightingUniform::s_normal, "s_normal");
			postLightingShader->registerUniform(PostLightingUniform::s_position, "s_position");
			postLightingShader->registerUniform(PostLightingUniform::lightPosition, "lightPosition");
			postLightingShader->registerUniform(PostLightingUniform::lightRange, "lightRange");
			postLightingShader->registerUniform(PostLightingUniform::lightColor, "lightColor");
			postLightingShader->use();
			postLightingShader->setUniform(PostLightingUniform::s_color, 0);
			postLightingShader->setUniform(PostLightingUniform::s_normal, 1);
			postLightingShader->setUniform(PostLightingUniform::s_position, 2);
			//TODO: change resolution of FBO to match target
			gbuffers = new vrlib::gl::FBO(2048, 2048, true, vrlib::gl::FBO::Color, vrlib::gl::FBO::Normal, vrlib::gl::FBO::Position);
		}

		void Renderer::update(float elapsedTime)
		{
			if (treeDirty)
			{
				updateRenderables();
				if (!cameraNode)
					cameraNode = findNodeWithComponent<components::Camera>();
				treeDirty = false;
			}

			fortree([this, &elapsedTime](Node* n)
			{
				for (Component* c : components)
					c->update(elapsedTime);
			});

			//TODO: update transform matrices

			std::function<void(Node*, const glm::mat4 &)> updateTransforms;
			updateTransforms = [this, &updateTransforms](Node* n, const glm::mat4 &parentTransform)
			{
				components::Transform* transform = n->getComponent<components::Transform>();
				if (transform)
				{
					transform->buildTransform();
					transform->globalTransform = parentTransform * transform->transform;
					for (auto c : n->children)
						updateTransforms(c, transform->globalTransform);
				}
				else
					for (auto c : n->children)
						updateTransforms(c, parentTransform);
			};
			updateTransforms(this, glm::mat4());
		}


		void Renderer::render(const glm::mat4 &projectionMatrix, const glm::mat4 &modelViewMatrix)
		{
			components::Camera* camera = cameraNode->getComponent<components::Camera>();

			renderShader->use();
			renderShader->setUniform(RenderUniform::projectionMatrix, projectionMatrix);
			renderShader->setUniform(RenderUniform::viewMatrix, modelViewMatrix);
			renderShader->setUniform(RenderUniform::diffuseColor, glm::vec4(1, 1, 1, 1));
			renderShader->setUniform(RenderUniform::textureFactor, 1.0f);

			//gbuffers->bind();

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			for (Node* c : renderables)
			{
				components::ModelRenderer* m = c->getComponent<components::ModelRenderer>();
				components::Transform* t = c->getComponent<components::Transform>();

				m->model->draw([this, t](const glm::mat4 &modelMatrix)
				{
					renderShader->setUniform(RenderUniform::modelMatrix, t->globalTransform * modelMatrix);
				},
				[this](const vrlib::Material &material)
				{
					if (material.texture)
					{
						renderShader->setUniform(RenderUniform::textureFactor, 1.0f);
						material.texture->bind();
					}
					else
					{
						renderShader->setUniform(RenderUniform::textureFactor, 0.0f);
						renderShader->setUniform(RenderUniform::diffuseColor, material.color.diffuse);
					}
				});
			}

			//gbuffers->unbind();


			//camera->target->bind();

			//camera->target->unbind();


		}


	}
}