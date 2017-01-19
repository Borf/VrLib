#include "Renderer.h"
#include "Scene.h"
#include "components/ModelRenderer.h"
#include "components/Transform.h"
#include "components/Camera.h"
#include "components/Light.h"
#include "components/SkyBox.h"

#include <VrLib/gl/FBO.h>
#include <VrLib/gl/VAO.h>
#include <VrLib/Model.h>
#include <VrLib/Texture.h>
#include <VrLib/gl/Vertex.h>
#include <VrLib/math/Frustum.h>

#include <btBulletCollisionCommon.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace vrlib
{
	namespace tien
	{
		Renderer::Renderer()
		{
			drawPhysicsDebug = false;
			drawLightDebug = false;
			drawMode = DrawMode::Default;
		}
		

		void Renderer::init()
		{
			postLightingShader = new vrlib::gl::Shader<PostLightingUniform>("data/vrlib/tien/shaders/postLighting.vert", "data/vrlib/tien/shaders/postLighting.frag");
			postLightingShader->bindAttributeLocation("a_position", 0);
			postLightingShader->link();
			postLightingShader->bindFragLocation("fragColor", 0);
			postLightingShader->registerUniform(PostLightingUniform::windowSize, "windowSize");
			postLightingShader->registerUniform(PostLightingUniform::windowPos, "windowPos");
			postLightingShader->registerUniform(PostLightingUniform::modelViewMatrix, "modelViewMatrix");
			postLightingShader->registerUniform(PostLightingUniform::projectionMatrix, "projectionMatrix");
			postLightingShader->registerUniform(PostLightingUniform::modelViewMatrixInv, "modelViewMatrixInv");
			postLightingShader->registerUniform(PostLightingUniform::projectionMatrixInv, "projectionMatrixInv");
			postLightingShader->registerUniform(PostLightingUniform::shadowMatrix, "shadowMatrix");

			postLightingShader->registerUniform(PostLightingUniform::s_color, "s_color");
			postLightingShader->registerUniform(PostLightingUniform::s_normal, "s_normal");
			postLightingShader->registerUniform(PostLightingUniform::s_depth, "s_depth");
			postLightingShader->registerUniform(PostLightingUniform::s_shadowmap, "s_shadowmap");
			postLightingShader->registerUniform(PostLightingUniform::s_shadowmapcube, "s_shadowmapcube");
			postLightingShader->registerUniform(PostLightingUniform::lightType, "lightType");
			postLightingShader->registerUniform(PostLightingUniform::lightPosition, "lightPosition");
			postLightingShader->registerUniform(PostLightingUniform::lightDirection, "lightDirection");
			postLightingShader->registerUniform(PostLightingUniform::lightAmbient, "lightAmbient");
			postLightingShader->registerUniform(PostLightingUniform::lightRange, "lightRange");
			postLightingShader->registerUniform(PostLightingUniform::lightCutoff, "lightCutoff");			
			postLightingShader->registerUniform(PostLightingUniform::lightIntensity, "lightIntensity");
			postLightingShader->registerUniform(PostLightingUniform::lightColor, "lightColor");
			postLightingShader->registerUniform(PostLightingUniform::lightSpotAngle, "lightSpotAngle");
			postLightingShader->registerUniform(PostLightingUniform::lightCastShadow, "lightCastShadow");
			postLightingShader->registerUniform(PostLightingUniform::cameraPosition, "cameraPosition");
			postLightingShader->registerUniform(PostLightingUniform::debug, "debug");
			
			postLightingShader->use();
			postLightingShader->setUniform(PostLightingUniform::s_color, 0);
			postLightingShader->setUniform(PostLightingUniform::s_normal, 1);
			postLightingShader->setUniform(PostLightingUniform::s_depth, 2);
			postLightingShader->setUniform(PostLightingUniform::s_shadowmap, 3);
			postLightingShader->setUniform(PostLightingUniform::s_shadowmapcube, 4);
			postLightingShader->setUniform(PostLightingUniform::debug, false);
			gbuffers = nullptr;



			postLightingStencilShader = new vrlib::gl::Shader<PostLightingStencilUniform>("data/vrlib/tien/shaders/postLighting.stencil.vert", "data/vrlib/tien/shaders/postLighting.stencil.frag");
			postLightingStencilShader->bindAttributeLocation("a_position", 0);
			postLightingStencilShader->link();
			postLightingStencilShader->bindFragLocation("fragColor", 0);
			postLightingStencilShader->registerUniform(PostLightingStencilUniform::modelViewMatrix, "modelViewMatrix");
			postLightingStencilShader->registerUniform(PostLightingStencilUniform::projectionMatrix, "projectionMatrix");
			postLightingStencilShader->registerUniform(PostLightingStencilUniform::lightType, "lightType");


			physicsDebugShader = new vrlib::gl::Shader<PhysicsDebugUniform>("data/vrlib/tien/shaders/physicsdebug.vert", "data/vrlib/tien/shaders/physicsdebug.frag");
			physicsDebugShader->bindAttributeLocation("a_position", 0);
			physicsDebugShader->bindAttributeLocation("a_color", 1);
			physicsDebugShader->link();
			physicsDebugShader->registerUniform(PhysicsDebugUniform::projectionMatrix, "projectionMatrix");
			physicsDebugShader->registerUniform(PhysicsDebugUniform::modelViewMatrix, "modelViewMatrix");


			simpleDebugShader = new vrlib::gl::Shader<SimpleDebugUniform>("data/vrlib/tien/shaders/simpledebug.vert", "data/vrlib/tien/shaders/simpledebug.frag");
			simpleDebugShader->bindAttributeLocation("a_position", 0);
			simpleDebugShader->bindAttributeLocation("a_texcoord", 1);
			simpleDebugShader->link();
			simpleDebugShader->bindFragLocation("fragColor", 0);
			simpleDebugShader->registerUniform(SimpleDebugUniform::projectionMatrix, "projectionMatrix");
			simpleDebugShader->registerUniform(SimpleDebugUniform::modelViewMatrix, "modelViewMatrix");
			simpleDebugShader->registerUniform(SimpleDebugUniform::s_texture, "s_texture");
			simpleDebugShader->registerUniform(SimpleDebugUniform::textureFactor, "textureFactor");
			simpleDebugShader->registerUniform(SimpleDebugUniform::color, "color");
			simpleDebugShader->registerUniform(SimpleDebugUniform::showAlpha, "showAlpha");

			buildOverlay();


			glGenTextures(1, &fakeDepthBuffer);
			glBindTexture(GL_TEXTURE_2D, fakeDepthBuffer);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, 1, 1, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);



			mHead.init("MainUserHead");

		}

		std::map<std::pair<Node*, int>, vrlib::gl::FBO*> gbufferMap;

		void Renderer::render(const Scene& scene, const glm::mat4 &projectionMatrix, const glm::mat4 &modelMatrix, Node* cameraNode, int renderId)
		{
			//first let's initialize the gbuffers
			int viewport[4];
			glGetIntegerv(GL_VIEWPORT, viewport);

			if (gbufferMap.find(std::pair<Node*, int>(cameraNode,renderId)) == gbufferMap.end())
				gbuffers = nullptr;
			else
				gbuffers = gbufferMap[std::pair<Node*, int>(cameraNode, renderId)];

			if (gbuffers && (gbuffers->getWidth() != viewport[2] ||
				gbuffers->getHeight() != viewport[3]))
			{
				logger << "Performance warning: gbuffer got resized" << Log::newline;
				logger << "Old size: " << gbuffers->getWidth() << ", " << gbuffers->getHeight() << Log::newline;
				logger << "New size: " << viewport[2] << ", " << viewport[3]<< Log::newline;
				delete gbuffers;
				gbuffers = nullptr;
			}
			if (!gbuffers)
			{
				gbuffers = new vrlib::gl::FBO(viewport[2], viewport[3], true, vrlib::gl::FBO::Color, vrlib::gl::FBO::Normal);
				gbufferMap[std::pair<Node*, int>(cameraNode, renderId)] = gbuffers;
			}



			//let's first update the camera stuff
			glm::mat4 modelViewMatrix = modelMatrix;
			if (cameraNode)
			{
				components::Camera* camera = cameraNode->getComponent<components::Camera>();
				modelViewMatrix = modelMatrix * glm::inverse(cameraNode->transform->globalTransform);
			}
			scene.frustum->setFromMatrix(projectionMatrix, modelViewMatrix);
			
			std::vector<vrlib::tien::Node*> visibleLights;
			for (auto l : scene.lights)
			{
				if (l->light->inFrustum(scene.frustum))
					visibleLights.push_back(l);
			}


			//update the lights / shadowmaps
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(1.0, 1.0f); //no idea what these values are or should be
			for (auto l : visibleLights)
			{
				if (!l->light)continue;
				if (l->light->shadow == components::Light::Shadow::shadowmap)
				{
					l->light->generateShadowMap();
				}
			}
			glPolygonOffset(0, 0);
			glDisable(GL_POLYGON_OFFSET_FILL);



			//fill the gbuffer
			gbuffers->bind();
			int oldFBO = gbuffers->oldFBO;
			glViewport(0, 0, gbuffers->getWidth(), gbuffers->getHeight());
			glClearColor(0, 0, 0, 1);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDisable(GL_BLEND);

			//call the setup for each rendercontext. This sets the projection matrix and view matrix
			for (components::Renderable::RenderContext* context : scene.renderContextsDeferred)
				context->frameSetup(projectionMatrix, modelViewMatrix);
			//and then actually draw to the gbuffer
			for (Node* c : scene.renderables)
			{
				auto r = c->getComponent<components::Renderable>();
				if (r->visible)
					r->drawDeferredPass();
			}
			gbuffers->unbind();

			//reset the old viewport, and clear it. Use scissoring to only clear this part of the viewport
			glScissor(viewport[0], viewport[1], viewport[2], viewport[3]);
			glEnable(GL_SCISSOR_TEST);
			glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
			glClearColor(0, 0, 0, 1);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glDisable(GL_SCISSOR_TEST);
			//copy the depth buffer (for lighting and later forward rendering)
			glBindFramebuffer(GL_READ_FRAMEBUFFER, gbuffers->fboId);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, oldFBO);
			glBlitFramebuffer(0, 0, gbuffers->getWidth(), gbuffers->getHeight(),
				viewport[0], viewport[1], viewport[0]+viewport[2], viewport[1]+viewport[3],
				GL_DEPTH_BUFFER_BIT, GL_NEAREST);

			glDisableVertexAttribArray(3);
			glDisableVertexAttribArray(4);
			glDisableVertexAttribArray(5);

			//draw the gbuffer in multiple passes. Every light needs a pass
			gbuffers->use();
			postLightingShader->use();
			postLightingShader->setUniform(PostLightingUniform::windowSize, glm::vec2(viewport[2], viewport[3]));
			postLightingShader->setUniform(PostLightingUniform::windowPos, glm::vec2(viewport[0], viewport[1]));
			postLightingShader->setUniform(PostLightingUniform::projectionMatrix, projectionMatrix);
			postLightingShader->setUniform(PostLightingUniform::projectionMatrixInv, glm::inverse(projectionMatrix));
			postLightingShader->setUniform(PostLightingUniform::modelViewMatrixInv, glm::inverse(modelViewMatrix));
			glm::vec3 cameraPosition(glm::inverse(modelViewMatrix) * glm::vec4(0, 0, 0, 1));
			postLightingShader->setUniform(PostLightingUniform::cameraPosition, cameraPosition );
			

			overlayVao->bind();
			glDisable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ONE);

			glDepthMask(GL_FALSE);
			glEnable(GL_CULL_FACE);
			glDisable(GL_DEPTH_TEST);
			glCullFace(GL_BACK);

			//every light adds shading to the scene, so draw the lights.
			//TODO: add instanced rendering for the visible lights
			//TODO: sort the visible lights to group them
			for (Node* c : visibleLights)
			{
				components::Light* l = c->getComponent<components::Light>();
				components::Transform* t = c->getComponent<components::Transform>();
				glm::vec3 pos(t->globalTransform * glm::vec4(0, 0, 0, 1));

			
				if (l->shadow == components::Light::Shadow::shadowmap && l->shadowMapDirectional)
				{
					if(l->type == components::Light::Type::directional || l->type == components::Light::Type::spot)
						l->shadowMapDirectional->use(3);
					else
						l->shadowMapDirectional->use(4);
					postLightingShader->setUniform(PostLightingUniform::shadowMatrix, l->projectionMatrix * l->modelViewMatrix);
					postLightingShader->setUniform(PostLightingUniform::lightCastShadow, true);
				}
				else
				{
					postLightingShader->setUniform(PostLightingUniform::lightCastShadow, false);

					glActiveTexture(GL_TEXTURE3);
					glBindTexture(GL_TEXTURE_2D, fakeDepthBuffer);
					glActiveTexture(GL_TEXTURE0);
				}



				float adjustedRange = l->realRange();
				postLightingShader->setUniform(PostLightingUniform::modelViewMatrix, glm::scale(glm::translate(modelViewMatrix, pos), glm::vec3(adjustedRange, adjustedRange, adjustedRange)));
				postLightingShader->setUniform(PostLightingUniform::lightType, (int)l->type);
				postLightingShader->setUniform(PostLightingUniform::lightPosition, pos);

				glm::vec3 lightDir(t->globalTransform * glm::vec4(1, 0, 0, 1) - t->globalTransform * glm::vec4(0, 0, 0,1));
				postLightingShader->setUniform(PostLightingUniform::lightDirection, -lightDir);

				postLightingShader->setUniform(PostLightingUniform::lightRange, l->range);
				postLightingShader->setUniform(PostLightingUniform::lightCutoff, l->cutoff);
				postLightingShader->setUniform(PostLightingUniform::lightIntensity, l->intensity);
				postLightingShader->setUniform(PostLightingUniform::lightColor, l->color);
				postLightingShader->setUniform(PostLightingUniform::lightSpotAngle, glm::radians(l->spotlightAngle/2.0f));
				postLightingShader->setUniform(PostLightingUniform::lightAmbient, l->directionalAmbient);

				if(l->type == components::Light::Type::directional || l->type == components::Light::Type::spot) //TODO: use mesh for spot
					glDrawArrays(GL_QUADS, 0, 4);
				else
				{
					postLightingStencilShader->use();
					postLightingStencilShader->setUniform(PostLightingStencilUniform::lightType, (int)l->type);
					postLightingStencilShader->setUniform(PostLightingStencilUniform::projectionMatrix, projectionMatrix);
					postLightingStencilShader->setUniform(PostLightingStencilUniform::modelViewMatrix, glm::scale(glm::translate(modelViewMatrix, pos), glm::vec3(adjustedRange, adjustedRange, adjustedRange)));

					glEnable(GL_STENCIL_TEST);
					glStencilFunc(GL_ALWAYS, 0x80, 0xFF);
					glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
					glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
					glClearStencil(0x80);
					glClear(GL_STENCIL_BUFFER_BIT);
					glColorMask(false, false, false, false);
					glDisable(GL_CULL_FACE);
					glEnable(GL_DEPTH_TEST);
					glDrawArrays(GL_TRIANGLES, sphere.x, sphere.y - sphere.x);
					glColorMask(true, true,true,true);
					postLightingShader->use();

					glEnable(GL_CULL_FACE);
					glCullFace(GL_FRONT);
					glDisable(GL_DEPTH_TEST);
					glStencilFunc(GL_NOTEQUAL, 0x80, 0xFF);
					glDrawArrays(GL_TRIANGLES, sphere.x, sphere.y - sphere.x);
					glDisable(GL_STENCIL_TEST);
				}
				glEnable(GL_BLEND);
			}
			glCullFace(GL_BACK);
			//done with all the deferred rendering, let's draw the forward rendered objects (objects with alpha)
			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_FALSE);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			//call the setup for each rendercontext. This sets the projection matrix and view matrix
			for (components::Renderable::RenderContext* context : scene.renderContextsForward)
				context->frameSetup(projectionMatrix, modelViewMatrix);
			for (Node* c : scene.renderables)
			{
				auto r = c->getComponent<components::Renderable>();
				if(r->visible)
					r->drawForwardPass();
			}




			//draw skybox (it's a special forward rendered object. Maybe this could be turned into a renderable?)
			vrlib::tien::components::SkyBox* skybox = nullptr;
			if(cameraNode)
				skybox = cameraNode->getComponent<vrlib::tien::components::SkyBox>();
			if (!skybox && scene.cameraNode)
				skybox = scene.cameraNode->getComponent<vrlib::tien::components::SkyBox>();
			if (!skybox && scene.findNodeWithComponent<vrlib::tien::components::SkyBox>())
				skybox = scene.findNodeWithComponent<vrlib::tien::components::SkyBox>()->getComponent<vrlib::tien::components::SkyBox>();


			if (skybox)
			{
				if (!skybox->initialized)
					skybox->initialize();
				skybox->render(projectionMatrix, modelViewMatrix);
			}

			glActiveTexture(GL_TEXTURE0);
			glDepthMask(GL_TRUE);
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDisable(GL_CULL_FACE);

			//We're done drawing, now let's add more stuff for debugging if needed
			if (drawPhysicsDebug)
			{
				scene.world->debugDrawWorld();
				if (scene.debugDrawer->verts.size() > 0)
				{
					glDisable(GL_DEPTH_TEST);
					glBindVertexArray(0);
					glBindBuffer(GL_ARRAY_BUFFER, 0);
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
					physicsDebugShader->use();
					physicsDebugShader->setUniform(PhysicsDebugUniform::modelViewMatrix, modelViewMatrix);
					physicsDebugShader->setUniform(PhysicsDebugUniform::projectionMatrix, projectionMatrix);

					gl::setAttributes<gl::VertexP3C4>(&scene.debugDrawer->verts[0]);
					glLineWidth(1.0f);
					glDrawArrays(GL_LINES, 0, scene.debugDrawer->verts.size());
					scene.debugDrawer->flush();
					glEnable(GL_DEPTH_TEST);
				}
			}
			
			//draw the boundingbox of the light. TODO: improve this code
			if (drawLightDebug)
			{
				std::vector<vrlib::gl::VertexP3C4> verts;
				for (auto l : visibleLights)
				{
					if (!l->light)continue;
					verts.push_back(vrlib::gl::VertexP3C4(glm::vec3(glm::inverse(l->light->projectionMatrix * l->light->modelViewMatrix) * glm::vec4( 0, 0,-1, 1)), glm::vec4(1, 0, 1, 1)));
					verts.push_back(vrlib::gl::VertexP3C4(glm::vec3(glm::inverse(l->light->projectionMatrix * l->light->modelViewMatrix) * glm::vec4(-2, 0,-1, 1)), glm::vec4(1, 0, 1, 1)));
					verts.push_back(vrlib::gl::VertexP3C4(glm::vec3(glm::inverse(l->light->projectionMatrix * l->light->modelViewMatrix) * glm::vec4(-2, 0,-1, 1)), glm::vec4(1, 0, 1, 1)));
					verts.push_back(vrlib::gl::VertexP3C4(glm::vec3(glm::inverse(l->light->projectionMatrix * l->light->modelViewMatrix) * glm::vec4(-2,-2,-1, 1)), glm::vec4(1, 0, 1, 1)));
					verts.push_back(vrlib::gl::VertexP3C4(glm::vec3(glm::inverse(l->light->projectionMatrix * l->light->modelViewMatrix) * glm::vec4(-2,-2,-1, 1)), glm::vec4(1, 0, 1, 1)));
					verts.push_back(vrlib::gl::VertexP3C4(glm::vec3(glm::inverse(l->light->projectionMatrix * l->light->modelViewMatrix) * glm::vec4( 0,-2,-1, 1)), glm::vec4(1, 0, 1, 1)));
					verts.push_back(vrlib::gl::VertexP3C4(glm::vec3(glm::inverse(l->light->projectionMatrix * l->light->modelViewMatrix) * glm::vec4( 0,-2,-1, 1)), glm::vec4(1, 0, 1, 1)));
					verts.push_back(vrlib::gl::VertexP3C4(glm::vec3(glm::inverse(l->light->projectionMatrix * l->light->modelViewMatrix) * glm::vec4( 0, 0,-1, 1)), glm::vec4(1, 0, 1, 1)));

					verts.push_back(vrlib::gl::VertexP3C4(glm::vec3(glm::inverse(l->light->projectionMatrix * l->light->modelViewMatrix) * glm::vec4( 0, 0, 1, 1)), glm::vec4(1, 0, 1, 1)));
					verts.push_back(vrlib::gl::VertexP3C4(glm::vec3(glm::inverse(l->light->projectionMatrix * l->light->modelViewMatrix) * glm::vec4(-2, 0, 1, 1)), glm::vec4(1, 0, 1, 1)));
					verts.push_back(vrlib::gl::VertexP3C4(glm::vec3(glm::inverse(l->light->projectionMatrix * l->light->modelViewMatrix) * glm::vec4(-2, 0, 1, 1)), glm::vec4(1, 0, 1, 1)));
					verts.push_back(vrlib::gl::VertexP3C4(glm::vec3(glm::inverse(l->light->projectionMatrix * l->light->modelViewMatrix) * glm::vec4(-2,-2, 1, 1)), glm::vec4(1, 0, 1, 1)));
					verts.push_back(vrlib::gl::VertexP3C4(glm::vec3(glm::inverse(l->light->projectionMatrix * l->light->modelViewMatrix) * glm::vec4(-2,-2, 1, 1)), glm::vec4(1, 0, 1, 1)));
					verts.push_back(vrlib::gl::VertexP3C4(glm::vec3(glm::inverse(l->light->projectionMatrix * l->light->modelViewMatrix) * glm::vec4( 0,-2, 1, 1)), glm::vec4(1, 0, 1, 1)));
					verts.push_back(vrlib::gl::VertexP3C4(glm::vec3(glm::inverse(l->light->projectionMatrix * l->light->modelViewMatrix) * glm::vec4( 0,-2, 1, 1)), glm::vec4(1, 0, 1, 1)));
					verts.push_back(vrlib::gl::VertexP3C4(glm::vec3(glm::inverse(l->light->projectionMatrix * l->light->modelViewMatrix) * glm::vec4( 0, 0, 1, 1)), glm::vec4(1, 0, 1, 1)));


					verts.push_back(vrlib::gl::VertexP3C4(glm::vec3(glm::inverse(l->light->projectionMatrix * l->light->modelViewMatrix) * glm::vec4( 0, 0, 1, 1)), glm::vec4(1, 0, 1, 1)));
					verts.push_back(vrlib::gl::VertexP3C4(glm::vec3(glm::inverse(l->light->projectionMatrix * l->light->modelViewMatrix) * glm::vec4( 0, 0,-1, 1)), glm::vec4(1, 0, 1, 1)));
					verts.push_back(vrlib::gl::VertexP3C4(glm::vec3(glm::inverse(l->light->projectionMatrix * l->light->modelViewMatrix) * glm::vec4(-2, 0, 1, 1)), glm::vec4(1, 0, 1, 1)));
					verts.push_back(vrlib::gl::VertexP3C4(glm::vec3(glm::inverse(l->light->projectionMatrix * l->light->modelViewMatrix) * glm::vec4(-2, 0,-1, 1)), glm::vec4(1, 0, 1, 1)));
					verts.push_back(vrlib::gl::VertexP3C4(glm::vec3(glm::inverse(l->light->projectionMatrix * l->light->modelViewMatrix) * glm::vec4( 0,-2, 1, 1)), glm::vec4(1, 0, 1, 1)));
					verts.push_back(vrlib::gl::VertexP3C4(glm::vec3(glm::inverse(l->light->projectionMatrix * l->light->modelViewMatrix) * glm::vec4( 0,-2,-1, 1)), glm::vec4(1, 0, 1, 1)));
					verts.push_back(vrlib::gl::VertexP3C4(glm::vec3(glm::inverse(l->light->projectionMatrix * l->light->modelViewMatrix) * glm::vec4(-2,-2, 1, 1)), glm::vec4(1, 0, 1, 1)));
					verts.push_back(vrlib::gl::VertexP3C4(glm::vec3(glm::inverse(l->light->projectionMatrix * l->light->modelViewMatrix) * glm::vec4(-2,-2,-1, 1)), glm::vec4(1, 0, 1, 1)));



				}


				glm::mat4 mat = glm::inverse(scene.frustum->projectionMatrix);
				auto drawVert = [&, this](const glm::vec3 &pos)
				{
					glm::vec4 p(mat * glm::vec4(pos, 1));
					p = glm::vec4(p.x * p.w, p.y * p.w, p.z * p.w, 1);
					p = glm::inverse(scene.frustum->modelviewMatrix) * p;
					verts.push_back(vrlib::gl::VertexP3C4(glm::vec3(p), glm::vec4(1, 0, 1, 1)));
				};

				drawVert(glm::vec3(1, 1, 0));
				drawVert(glm::vec3(-1, 1, 0));
				drawVert(glm::vec3(-1, 1, 0));
				drawVert(glm::vec3(-1, -1, 0));
				drawVert(glm::vec3(-1, -1, 0));
				drawVert(glm::vec3(1, -1, 0));
				drawVert(glm::vec3(1, -1, 0));
				drawVert(glm::vec3(1, 1, 0));

				drawVert(glm::vec3(1, 1, 1));
				drawVert(glm::vec3(-1, 1, 1));
				drawVert(glm::vec3(-1, 1, 1));
				drawVert(glm::vec3(-1, -1, 1));
				drawVert(glm::vec3(-1, -1, 1));
				drawVert(glm::vec3(1, -1, 1));
				drawVert(glm::vec3(1, -1, 1));
				drawVert(glm::vec3(1, 1, 1));


				drawVert(glm::vec3(1, 1, 1));
				drawVert(glm::vec3(1, 1, 0));
				drawVert(glm::vec3(-1, 1, 1));
				drawVert(glm::vec3(-1, 1, 0));
				drawVert(glm::vec3(1, -1, 1));
				drawVert(glm::vec3(1, -1, 0));
				drawVert(glm::vec3(-1, -1, 1));
				drawVert(glm::vec3(-1, -1, 0));



				if (!verts.empty())
				{
					//glDisable(GL_DEPTH_TEST);
					glBindVertexArray(0);
					glBindBuffer(GL_ARRAY_BUFFER, 0);
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
					physicsDebugShader->use();
					physicsDebugShader->setUniform(PhysicsDebugUniform::modelViewMatrix, modelViewMatrix);
					physicsDebugShader->setUniform(PhysicsDebugUniform::projectionMatrix, projectionMatrix);


					gl::setAttributes<gl::VertexP3C4>(&verts[0]);
					glLineWidth(4.0f);
					glDrawArrays(GL_LINES, 0, verts.size());
					scene.debugDrawer->flush();
				}
				glEnable(GL_DEPTH_TEST);
			}
			//camera->target->bind();
			//camera->target->unbind();


			glDisable(GL_BLEND);
			if (drawMode != DrawMode::Default)
			{

				vrlib::gl::VertexP3T2 verts[] = {
					vrlib::gl::VertexP3T2(glm::vec3(-1,-1,0), glm::vec2(0,0)),
					vrlib::gl::VertexP3T2(glm::vec3(1,-1,0), glm::vec2(1,0)),
					vrlib::gl::VertexP3T2(glm::vec3(1,1,0), glm::vec2(1,1)),
					vrlib::gl::VertexP3T2(glm::vec3(-1,1,0), glm::vec2(0,1))
				};
				glBindVertexArray(0);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

				simpleDebugShader->use();
				simpleDebugShader->setUniform(SimpleDebugUniform::modelViewMatrix, glm::mat4());
				simpleDebugShader->setUniform(SimpleDebugUniform::projectionMatrix, glm::mat4());
				simpleDebugShader->setUniform(SimpleDebugUniform::textureFactor, 1.0f);
				simpleDebugShader->setUniform(SimpleDebugUniform::s_texture, 0);
				simpleDebugShader->setUniform(SimpleDebugUniform::color, glm::vec4(1, 1, 1, 1));
				simpleDebugShader->setUniform(SimpleDebugUniform::showAlpha, false);

				if (drawMode == DrawMode::Albedo)
					glBindTexture(GL_TEXTURE_2D, gbuffers->texid[0]);
				if (drawMode == DrawMode::Normals)
					glBindTexture(GL_TEXTURE_2D, gbuffers->texid[1]);
				if (drawMode == DrawMode::Lightmaps)
				{
					auto light = scene.lights[debugLightMapIndex]->getComponent<vrlib::tien::components::Light>();
					if(light->shadowMapDirectional)
						light->shadowMapDirectional->use();
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
					glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
				}
				if (drawMode == DrawMode::Specular)
				{
					glBindTexture(GL_TEXTURE_2D, gbuffers->texid[1]);
					simpleDebugShader->setUniform(SimpleDebugUniform::showAlpha, true);
				}


				vrlib::gl::setAttributes<vrlib::gl::VertexP3T2>(verts);
				glDisable(GL_BLEND);
				glDisable(GL_DEPTH_TEST);
				glDrawArrays(GL_QUADS, 0, 4);




				if (drawMode == DrawMode::Lightmaps)
				{
					auto light = scene.lights[debugLightMapIndex]->getComponent<vrlib::tien::components::Light>();
					if(light->shadowMapDirectional)
						light->shadowMapDirectional->use();
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
				}

			}
		}


		void Renderer::buildOverlay()
		{
			std::vector<vrlib::gl::VertexP3> verts;
			vrlib::gl::VertexP3 vert;
			vrlib::gl::setP3(vert, glm::vec3(-1, -1, 0));	verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(1, -1, 0));	verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(1, 1, 0));	verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-1, 1, 0));	verts.push_back(vert);
			sphere.x = verts.size();
			vrlib::gl::setP3(vert, glm::vec3(0.000000, 0.000000, -1.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.425323, -0.309011, -0.850654)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.162456, -0.499995, -0.850654)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.723607, -0.525725, -0.447220)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.425323, -0.309011, -0.850654)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.850648, 0.000000, -0.525736)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.000000, 0.000000, -1.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.162456, -0.499995, -0.850654)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.525730, 0.000000, -0.850652)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.000000, 0.000000, -1.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.525730, 0.000000, -0.850652)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.162456, 0.499995, -0.850654)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.000000, 0.000000, -1.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.162456, 0.499995, -0.850654)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.425323, 0.309011, -0.850654)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.723607, -0.525725, -0.447220)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.850648, 0.000000, -0.525736)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.951058, -0.309013, 0.000000)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.276388, -0.850649, -0.447220)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.262869, -0.809012, -0.525738)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.000000, -1.000000, 0.000000)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.894426, 0.000000, -0.447216)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.688189, -0.499997, -0.525736)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.951058, -0.309013, 0.000000)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.276388, 0.850649, -0.447220)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.688189, 0.499997, -0.525736)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.587786, 0.809017, 0.000000)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.723607, 0.525725, -0.447220)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.262869, 0.809012, -0.525738)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.587786, 0.809017, 0.000000)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.723607, -0.525725, -0.447220)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.951058, -0.309013, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.587786, -0.809017, 0.000000)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.276388, -0.850649, -0.447220)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.000000, -1.000000, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.587786, -0.809017, 0.000000)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.894426, 0.000000, -0.447216)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.951058, -0.309013, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.951058, 0.309013, 0.000000)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.276388, 0.850649, -0.447220)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.587786, 0.809017, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.000000, 1.000000, 0.000000)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.723607, 0.525725, -0.447220)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.587786, 0.809017, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.951058, 0.309013, 0.000000)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.276388, -0.850649, 0.447220)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.688189, -0.499997, 0.525736)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.162456, -0.499995, 0.850654)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.723607, -0.525725, 0.447220)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.262869, -0.809012, 0.525738)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.425323, -0.309011, 0.850654)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.723607, 0.525725, 0.447220)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.850648, 0.000000, 0.525736)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.425323, 0.309011, 0.850654)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.276388, 0.850649, 0.447220)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.262869, 0.809012, 0.525738)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.162456, 0.499995, 0.850654)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.894426, 0.000000, 0.447216)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.688189, 0.499997, 0.525736)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.525730, 0.000000, 0.850652)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.525730, 0.000000, 0.850652)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.162456, 0.499995, 0.850654)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.000000, 0.000000, 1.000000)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.525730, 0.000000, 0.850652)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.688189, 0.499997, 0.525736)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.162456, 0.499995, 0.850654)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.688189, 0.499997, 0.525736)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.276388, 0.850649, 0.447220)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.162456, 0.499995, 0.850654)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.162456, 0.499995, 0.850654)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.425323, 0.309011, 0.850654)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.000000, 0.000000, 1.000000)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.162456, 0.499995, 0.850654)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.262869, 0.809012, 0.525738)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.425323, 0.309011, 0.850654)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.262869, 0.809012, 0.525738)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.723607, 0.525725, 0.447220)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.425323, 0.309011, 0.850654)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.425323, 0.309011, 0.850654)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.425323, -0.309011, 0.850654)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.000000, 0.000000, 1.000000)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.425323, 0.309011, 0.850654)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.850648, 0.000000, 0.525736)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.425323, -0.309011, 0.850654)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.850648, 0.000000, 0.525736)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.723607, -0.525725, 0.447220)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.425323, -0.309011, 0.850654)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.425323, -0.309011, 0.850654)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.162456, -0.499995, 0.850654)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.000000, 0.000000, 1.000000)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.425323, -0.309011, 0.850654)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.262869, -0.809012, 0.525738)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.162456, -0.499995, 0.850654)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.262869, -0.809012, 0.525738)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.276388, -0.850649, 0.447220)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.162456, -0.499995, 0.850654)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.162456, -0.499995, 0.850654)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.525730, 0.000000, 0.850652)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.000000, 0.000000, 1.000000)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.162456, -0.499995, 0.850654)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.688189, -0.499997, 0.525736)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.525730, 0.000000, 0.850652)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.688189, -0.499997, 0.525736)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.894426, 0.000000, 0.447216)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.525730, 0.000000, 0.850652)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.951058, 0.309013, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.688189, 0.499997, 0.525736)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.894426, 0.000000, 0.447216)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.951058, 0.309013, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.587786, 0.809017, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.688189, 0.499997, 0.525736)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.587786, 0.809017, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.276388, 0.850649, 0.447220)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.688189, 0.499997, 0.525736)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.000000, 1.000000, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.262869, 0.809012, 0.525738)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.276388, 0.850649, 0.447220)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.000000, 1.000000, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.587786, 0.809017, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.262869, 0.809012, 0.525738)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.587786, 0.809017, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.723607, 0.525725, 0.447220)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.262869, 0.809012, 0.525738)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.951058, 0.309013, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.850648, 0.000000, 0.525736)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.723607, 0.525725, 0.447220)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.951058, 0.309013, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.951058, -0.309013, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.850648, 0.000000, 0.525736)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.951058, -0.309013, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.723607, -0.525725, 0.447220)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.850648, 0.000000, 0.525736)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.587786, -0.809017, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.262869, -0.809012, 0.525738)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.723607, -0.525725, 0.447220)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.587786, -0.809017, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.000000, -1.000000, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.262869, -0.809012, 0.525738)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.000000, -1.000000, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.276388, -0.850649, 0.447220)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.262869, -0.809012, 0.525738)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.587786, -0.809017, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.688189, -0.499997, 0.525736)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.276388, -0.850649, 0.447220)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.587786, -0.809017, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.951058, -0.309013, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.688189, -0.499997, 0.525736)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.951058, -0.309013, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.894426, 0.000000, 0.447216)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.688189, -0.499997, 0.525736)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.587786, 0.809017, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.000000, 1.000000, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.276388, 0.850649, 0.447220)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.587786, 0.809017, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.262869, 0.809012, -0.525738)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.000000, 1.000000, 0.000000)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.262869, 0.809012, -0.525738)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.276388, 0.850649, -0.447220)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.000000, 1.000000, 0.000000)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.587786, 0.809017, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.951058, 0.309013, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.723607, 0.525725, 0.447220)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.587786, 0.809017, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.688189, 0.499997, -0.525736)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.951058, 0.309013, 0.000000)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.688189, 0.499997, -0.525736)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.894426, 0.000000, -0.447216)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.951058, 0.309013, 0.000000)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.951058, -0.309013, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.587786, -0.809017, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.723607, -0.525725, 0.447220)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.951058, -0.309013, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.688189, -0.499997, -0.525736)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.587786, -0.809017, 0.000000)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.688189, -0.499997, -0.525736)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.276388, -0.850649, -0.447220)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.587786, -0.809017, 0.000000)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.000000, -1.000000, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.587786, -0.809017, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.276388, -0.850649, 0.447220)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.000000, -1.000000, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.262869, -0.809012, -0.525738)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.587786, -0.809017, 0.000000)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.262869, -0.809012, -0.525738)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.723607, -0.525725, -0.447220)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.587786, -0.809017, 0.000000)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.951058, -0.309013, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.951058, 0.309013, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.894426, 0.000000, 0.447216)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.951058, -0.309013, 0.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.850648, 0.000000, -0.525736)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.951058, 0.309013, 0.000000)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.850648, 0.000000, -0.525736)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.723607, 0.525725, -0.447220)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.951058, 0.309013, 0.000000)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.425323, 0.309011, -0.850654)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.262869, 0.809012, -0.525738)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.723607, 0.525725, -0.447220)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.425323, 0.309011, -0.850654)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.162456, 0.499995, -0.850654)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.262869, 0.809012, -0.525738)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.162456, 0.499995, -0.850654)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.276388, 0.850649, -0.447220)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.262869, 0.809012, -0.525738)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.162456, 0.499995, -0.850654)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.688189, 0.499997, -0.525736)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.276388, 0.850649, -0.447220)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.162456, 0.499995, -0.850654)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.525730, 0.000000, -0.850652)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.688189, 0.499997, -0.525736)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.525730, 0.000000, -0.850652)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.894426, 0.000000, -0.447216)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.688189, 0.499997, -0.525736)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.525730, 0.000000, -0.850652)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.688189, -0.499997, -0.525736)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.894426, 0.000000, -0.447216)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.525730, 0.000000, -0.850652)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.162456, -0.499995, -0.850654)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.688189, -0.499997, -0.525736)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.162456, -0.499995, -0.850654)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.276388, -0.850649, -0.447220)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.688189, -0.499997, -0.525736)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.850648, 0.000000, -0.525736)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.425323, 0.309011, -0.850654)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.723607, 0.525725, -0.447220)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.850648, 0.000000, -0.525736)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.425323, -0.309011, -0.850654)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.425323, 0.309011, -0.850654)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.425323, -0.309011, -0.850654)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.000000, 0.000000, -1.000000)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.425323, 0.309011, -0.850654)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.162456, -0.499995, -0.850654)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.262869, -0.809012, -0.525738)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(-0.276388, -0.850649, -0.447220)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(-0.162456, -0.499995, -0.850654)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.425323, -0.309011, -0.850654)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.262869, -0.809012, -0.525738)); verts.push_back(vert);

			vrlib::gl::setP3(vert, glm::vec3(0.425323, -0.309011, -0.850654)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.723607, -0.525725, -0.447220)); verts.push_back(vert);
			vrlib::gl::setP3(vert, glm::vec3(0.262869, -0.809012, -0.525738)); verts.push_back(vert);

			sphere.y = verts.size();
			overlayVerts = new vrlib::gl::VBO<vrlib::gl::VertexP3>();
			overlayVerts->setData(verts.size(), verts.data(), GL_STATIC_DRAW);
			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
			glDisableVertexAttribArray(2);
			glDisableVertexAttribArray(3);
			overlayVao = new vrlib::gl::VAO(overlayVerts);
		}


	}
}