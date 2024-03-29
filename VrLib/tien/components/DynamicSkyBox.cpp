#include "DynamicSkyBox.h"
#include "Light.h"
#include "Transform.h"
#include "../Renderer.h"
#include <VrLib/tien/Node.h>
#include <VrLib/Texture.h>
#include <VrLib/Model.h>
#include <VrLib/gl/Vertex.h>
#include <VrLib/json.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace vrlib
{
	namespace tien
	{
		namespace components
		{
			DynamicSkyBox::DynamicSkyBox(const nlohmann::json & json)
			{
				timeOfDay = json["timeOfDay"];
				//TODO: light
			}


			nlohmann::json DynamicSkyBox::toJson(nlohmann::json &meshes) const
			{
				nlohmann::json ret;
				ret["type"] = "dynamicskybox";
				ret["timeOfDay"] = timeOfDay;
				ret["light"] = light ? light->guid : "";
				return ret;
			}
			void DynamicSkyBox::initialize()
			{
				skydome = vrlib::Model::getModel<vrlib::gl::VertexP3>("data/vrlib/tien/models/skydome.obj");
				skydomeShader = new vrlib::gl::Shader<SkydomeUniforms>("data/vrlib/tien/shaders/DynamicSkyBox.skydome.vert", "data/vrlib/tien/shaders/DynamicSkyBox.skydome.frag");
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
				billboardShader = new vrlib::gl::Shader<BillboardUniforms>("data/vrlib/tien/shaders/DynamicSkyBox.billboard.vert", "data/vrlib/tien/shaders/DynamicSkyBox.billboard.frag");
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

			void DynamicSkyBox::update(float elapsedTime, Scene & scene)
			{
				if (light)
				{
					float now = (timeOfDay / 24) * glm::two_pi<float>() + glm::pi<float>();
					glm::vec3 sunDirection(0, cos(now), sin(now));
					float k = glm::cos(now);
					light->getComponent<Transform>()->position = sunDirection;
					if (k < 0)
					{
						light->getComponent<Transform>()->position = -sunDirection;
						light->getComponent<Light>()->color = -k * glm::vec4(0.2, 0.2, 0.3, 1);
					}
					else
					{
						light->getComponent<Transform>()->position = sunDirection;
						light->getComponent<Light>()->color = k * glm::vec4(1, 1, 0.8627f, 1);
					}
					//todo: use proper ambient and diffuse, not just color
				}
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
				Renderer::drawCalls++;

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
						Renderer::drawCalls++;
						return true;
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
						Renderer::drawCalls++;
						return true;
					});
				}
				glDepthMask(1);


			}


			void DynamicSkyBox::buildEditor(EditorBuilder * builder, bool folded)
			{
				builder->addTitle("Dynamic skybox");
				if (folded)
					return;

				builder->beginGroup("Time");
				builder->addTextBox(builder->toString(timeOfDay), [this](const std::string & newValue) { timeOfDay = (float)atof(newValue.c_str());  });
				builder->endGroup();

			}

		}
	}
}

