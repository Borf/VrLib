#include "Light.h"
#include "Transform.h"

#include <VrLib/gl/FBO.h>
#include <VrLib/Log.h>
#include "../Node.h"
#include "../Scene.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using vrlib::Log;

namespace vrlib
{
	namespace tien
	{
		namespace components
		{
			struct CameraDirection
			{
				GLenum cubemapFace;
				glm::vec3 target;
				glm::vec3 up;
			};

			CameraDirection gCameraDirections[6] =
			{
				{ GL_TEXTURE_CUBE_MAP_POSITIVE_X, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f) },
				{ GL_TEXTURE_CUBE_MAP_NEGATIVE_X, glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f) },
				{ GL_TEXTURE_CUBE_MAP_POSITIVE_Y, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f) },
				{ GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f) },
				{ GL_TEXTURE_CUBE_MAP_POSITIVE_Z, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f) },
				{ GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f) }
			};

			void Light::generateShadowMap()
			{
				if (shadow != Shadow::shadowmap)
					return;

				if (type == Type::directional)
				{
					if (!shadowMapDirectional)
						shadowMapDirectional = new vrlib::gl::FBO(1024*8, 1024*8, true, 0, true); //shadowmap

					projectionMatrix = glm::ortho(-8.0f, 8.0f, -8.0f, 8.0f, 0.0f, 10.0f);
					modelViewMatrix = glm::lookAt(glm::vec3(0, 5, 0), glm::vec3(0,5,0) - node->transform->position, glm::vec3(0, 1, 0));
					Scene& scene = node->getScene();

					shadowMapDirectional->bind();
					glViewport(0, 0, shadowMapDirectional->getWidth(), shadowMapDirectional->getHeight());
					glClearColor(1, 0, 0, 1);
					glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
					glEnable(GL_CULL_FACE);
					glCullFace(GL_BACK);

					for (components::Renderable::RenderContext* context : scene.renderContextsShadow)
					{
						context->frameSetup(projectionMatrix, modelViewMatrix);
						context->useCubemap(false);
					}

					for (Node* c : scene.renderables)
						c->getComponent<components::Renderable>()->drawShadowMap();

					shadowMapDirectional->unbind();
				}
				else if (type == Type::point)
				{
					if (!shadowMapDirectional)
						shadowMapDirectional = new vrlib::gl::FBO(1024*2, 1024*2, false, vrlib::gl::FBO::Type::ShadowCube); //shadowmap

					Scene& scene = node->getScene();

					glViewport(0, 0, shadowMapDirectional->getWidth(), shadowMapDirectional->getHeight());
					glEnable(GL_CULL_FACE);
					glCullFace(GL_BACK);

					for (int i = 0; i < 6; i++)
					{
						projectionMatrix = glm::perspective(glm::radians(90.0f), 1.0f, .1f, 50.0f);
						modelViewMatrix = glm::lookAt(node->transform->position, node->transform->position + gCameraDirections[i].target, gCameraDirections[i].up);
						shadowMapDirectional->bind(i);
						glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
						for (components::Renderable::RenderContext* context : scene.renderContextsShadow)
						{
							context->frameSetup(projectionMatrix, modelViewMatrix);
							context->useCubemap(true);
						}

						for (Node* c : scene.renderables)
							c->getComponent<components::Renderable>()->drawShadowMap();
					}

						shadowMapDirectional->unbind();
				}



			}
		}
	}
}