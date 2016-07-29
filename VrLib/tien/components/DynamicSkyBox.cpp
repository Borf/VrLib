#include "DynamicSkyBox.h"

#include <VrLib/Texture.h>
#include <VrLib/Model.h>
#include <VrLib/gl/Vertex.h>
#include <VrLib/json.h>
#include <glm/gtc/matrix_transform.hpp>

namespace vrlib
{
	namespace tien
	{
		namespace components
		{
			void DynamicSkyBox::initialize()
			{
				skydome = vrlib::Model::getModel<vrlib::gl::VertexP3>("data/vrlib/tien/models/skydome.obj");
				skydomeShader = new vrlib::gl::Shader<SkydomeUniforms>("data/vrlib/tien/shaders/skydome.vert", "data/vrlib/tien/shaders/skydome.frag");
				skydomeShader->bindAttributeLocation("a_position", 0);
				skydomeShader->link();
				skydomeShader->bindFragLocation("fragColor", 0);
				skydomeShader->registerUniform(SkydomeUniforms::projectionMatrix, "projectionMatrix");
				skydomeShader->registerUniform(SkydomeUniforms::modelViewMatrix, "modelViewMatrix");
				skydomeShader->registerUniform(SkydomeUniforms::glow, "glow");
				skydomeShader->registerUniform(SkydomeUniforms::color, "color");
				skydomeShader->registerUniform(SkydomeUniforms::sunDirection, "sunDirection");
				skydomeShader->use();
				skydomeShader->setUniform(SkydomeUniforms::color, 0);
				skydomeShader->setUniform(SkydomeUniforms::glow, 1);
				skydomeColor = vrlib::Texture::loadCached("data/vrlib/tien/Textures/sky.png");
				skydomeColor->setTextureRepeat(false);
				skydomeGlow = vrlib::Texture::loadCached("data/vrlib/tien/Textures/glow.png");
				skydomeGlow->setTextureRepeat(false);

				sun = vrlib::Model::getModel<vrlib::gl::VertexP3N3T2>("data/vrlib/tien/models/sun.obj");
				moon = vrlib::Model::getModel<vrlib::gl::VertexP3N3T2>("data/vrlib/tien/models/moon.obj");
				billboardShader = new vrlib::gl::Shader<BillboardUniforms>("data/vrlib/tien/shaders/billboard.vert", "data/vrlib/tien/shaders/billboard.frag");
				billboardShader->bindAttributeLocation("a_position", 0);
				billboardShader->bindAttributeLocation("a_texcoord", 2);
				billboardShader->link();
				billboardShader->bindFragLocation("fragColor", 0);
				billboardShader->registerUniform(BillboardUniforms::projectionMatrix, "projectionMatrix");
				billboardShader->registerUniform(BillboardUniforms::mat, "mat");
				billboardShader->registerUniform(BillboardUniforms::s_texture, "s_texture");
				billboardShader->use();
				billboardShader->setUniform(BillboardUniforms::s_texture, 0);
				initialized = true;
			}

			void DynamicSkyBox::render(const glm::mat4 & projectionMatrix, const glm::mat4 & modelviewMatrix)
			{
				glDepthMask(GL_TRUE);
				glEnable(GL_DEPTH_TEST);
				glDepthMask(0);

				skydomeShader->use();
				skydomeShader->setUniform(SkydomeUniforms::projectionMatrix, projectionMatrix);
				skydomeShader->setUniform(SkydomeUniforms::modelViewMatrix, glm::scale(modelviewMatrix, glm::vec3(7.5, 7.5, 7.5))); // model is 50 big

				glActiveTexture(GL_TEXTURE1);
				skydomeGlow->bind();
				glActiveTexture(GL_TEXTURE0);
				skydomeColor->bind();


				float now = (timeOfDay / 24) * glm::two_pi<float>() + glm::pi<float>();

				glm::vec3 sunDirection(0, cos(now), sin(now));

				//lights.front()->getComponent<components::Transform>()->position = sunDirection;
				//glm::vec3 sunDirection(0, 1, -1);
				sunDirection = glm::normalize(sunDirection);

				skydomeShader->setUniform(SkydomeUniforms::sunDirection, sunDirection);

				float k = float(glm::max(+glm::cos(glm::radians(now)), 0.0f));
				float b = float(glm::max(-glm::cos(glm::radians(now)), 0.0f));

				float A[4] = { 0.5f * k,
					0.5f * k,
					0.7f * k, 0.0f };

				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

				skydome->draw([](const glm::mat4 &mat) {});

				glm::vec3 cameraPos(modelviewMatrix * glm::vec4(0, 0, 0, 1));
				glm::vec3 pos = 4 * 45.0f * sunDirection;


				glDisable(GL_CULL_FACE);
				billboardShader->use();
				billboardShader->setUniform(BillboardUniforms::projectionMatrix, projectionMatrix);

				{
					glm::mat4 mat = modelviewMatrix;
					mat = glm::rotate(mat, now + glm::half_pi<float>(), glm::vec3(1, 0, 0));
					mat = glm::translate(mat, glm::vec3(0, 0, -400));
					mat = glm::scale(mat, glm::vec3(5, 5, 5));
					billboardShader->setUniform(BillboardUniforms::mat, mat);
					sun->draw([](const glm::mat4 &mat) {}, [this](const Material& material) {
						material.texture->bind();
					});
				}

				{
					glm::mat4 mat = modelviewMatrix;
					mat = glm::rotate(mat, now - glm::half_pi<float>(), glm::vec3(1, 0, 0));
					mat = glm::translate(mat, glm::vec3(0, 0, -300));
					mat = glm::scale(mat, glm::vec3(7.5, 7.5, 7.5));
					billboardShader->setUniform(BillboardUniforms::mat, mat);
					moon->draw([](const glm::mat4 &mat) {}, [this](const Material& material) {
						material.texture->bind();
					});
				}
				glDepthMask(1);


			}

			json::Value DynamicSkyBox::toJson() const
			{
				json::Value ret;
				ret["type"] = "camera";
				return ret;
			}
		}
	}
}

