#include "Light.h"
#include "Transform.h"

#include <VrLib/gl/FBO.h>
#include <VrLib/Log.h>
#include <VrLib/json.hpp>
#include <VrLib/math/Frustum.h>
#include "../Renderer.h"
#include "../Node.h"
#include "../Scene.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <sstream>

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
			


			Light::Light(const json & data)
			{
				if (data["lighttype"] == "directional")	type = Type::directional;
				if (data["lighttype"] == "point")		type = Type::point;
				if (data["lighttype"] == "spot")		type = Type::spot;

				if (data["shadow"] == "none")			shadow = Shadow::none;
				if (data["shadow"] == "shadowmap")		shadow = Shadow::shadowmap;
				if (data["shadow"] == "shadowvolume")	shadow = Shadow::shadowvolume;

				if (data.find("baking") != data.end())
				{
					if (data["baking"] == "baked")			baking = Baking::baked;
					if (data["baking"] == "realtime")		baking = Baking::realtime;
				}

				intensity = data["intensity"];
				range = data["range"];
				for (int i = 0; i < 4; i++)
					color[i] = data["color"][i];
				if(data.find("spotlightAngle") != data.end())
					spotlightAngle = data["spotlightAngle"];
				if (data.find("directionalAmbient") != data.end())
					directionalAmbient = data["directionalAmbient"];
				if (data.find("cutoff") != data.end())
					cutoff = data["cutoff"];
			}
			json Light::toJson(json &meshes) const
			{
				json ret;
				ret["type"] = "light";

				switch (type)
				{
				case Type::directional:		ret["lighttype"] = "directional";	break;
				case Type::point:			ret["lighttype"] = "point";			break;
				case Type::spot:			ret["lighttype"] = "spot";			break;
				default:
					ret["lighttype"] = "error";
				}

				switch (shadow)
				{
				case Shadow::none:			ret["shadow"] = "none";				break;
				case Shadow::shadowmap:		ret["shadow"] = "shadowmap";		break;
				case Shadow::shadowvolume:	ret["shadow"] = "shadowvolume";		break;
				default:
					ret["shadow"] = "error";
				}
				
				switch (baking)
				{
				case Baking::baked:			ret["baking"] = "baked";			break;
				case Baking::realtime:		ret["baking"] = "realtime";			break;
				default:
					ret["baking"] = "error";
				}
				ret["intensity"] = intensity;
				for (int i = 0; i < 4; i++)
					ret["color"].push_back(color[i]);

				ret["range"] = range;
				ret["spotlightAngle"] = spotlightAngle;
				ret["directionalAmbient"] = directionalAmbient;
				ret["cutoff"] = cutoff;

				return ret;
			}
			Light::~Light()
			{
				if (shadowMapDirectional)
					delete shadowMapDirectional;
				shadowMapDirectional = nullptr;
			}


			void Light::generateShadowMap()
			{
				if (shadow != Shadow::shadowmap)
					return;

				if (baking == Baking::baked && shadowmapGenerated)
					return;

				if (type == Type::directional || type == Type::spot)
				{
					if (!shadowMapDirectional)
						if (type == Type::directional)
							shadowMapDirectional = new vrlib::gl::FBO(1024*4, 1024*4, true, 0, true); //shadowmap
						else
							shadowMapDirectional = new vrlib::gl::FBO(512, 512, true, 0, true); //shadowmap for spotlights

					float size = 5.0f * node->transform->scale.x;

					glm::vec3 frustumCenter = node->getScene().frustum->getCenter(); //todo: cache?
					glm::vec3 eyePos = glm::vec3(glm::inverse(node->getScene().frustum->modelviewMatrix) * glm::vec4(0, 0, 0, 1));
					glm::vec3 dir = frustumCenter - eyePos;
					glm::vec3 lightDir(node->transform->globalTransform * glm::vec4(1, 0, 0, 1) - node->transform->globalTransform * glm::vec4(0, 0, 0, 1));


					frustumCenter = eyePos + size * 0.5f * glm::normalize(dir);
					//printf("Eyepos:\t%f\t%f\t%f\n", eyePos.x, eyePos.y, eyePos.z);
					//printf("dir:   \t%f\t%f\t%f\n", dir.x, dir.y, dir.z);

					glm::vec3 lightPosition = frustumCenter - 50.0f * lightDir;

					if (type == Type::directional)
						projectionMatrix = glm::ortho(-size, size, -size, size, 0.0f, 250.0f * size * .5f); //TODO: auto generate depth
					else
						projectionMatrix = glm::perspective(glm::radians(spotlightAngle), 1.0f, .1f, range); //TODO: test if range works


					if(type == Type::directional)
						modelViewMatrix = glm::lookAt(lightPosition, lightPosition + lightDir, glm::vec3(0, 1, 0));
					else
						modelViewMatrix = glm::lookAt(node->transform->getGlobalPosition(), node->transform->getGlobalPosition() + lightDir, glm::vec3(0, 1, 0));

					Scene& scene = node->getScene();

					shadowMapDirectional->bind();
					glViewport(0, 0, shadowMapDirectional->getWidth(), shadowMapDirectional->getHeight());
					glClearColor(1, 0, 0, 1);
					Renderer::drawCalls++;
					glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
					glDisable(GL_CULL_FACE);
					glCullFace(GL_BACK);

					for (components::Renderable::RenderContext* context : scene.renderContextsShadow)
					{
						context->frameSetup(projectionMatrix, modelViewMatrix);
						context->useCubemap(false);
					}

					glCullFace(GL_FRONT);
					for (auto renderable : scene.allRenderables)
					{
						if(renderable->visible && renderable->node->enabled)
							renderable->drawShadowMap();
					}
					glCullFace(GL_BACK);
					shadowMapDirectional->unbind();
					shadowmapGenerated = true;
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

						for (auto renderable : scene.allRenderables)
						{
							if (renderable->visible && renderable->node->enabled)
								renderable->drawShadowMap();
						}
					}
					shadowMapDirectional->unbind();
					shadowmapGenerated = true;
				}



			}


			void Light::buildEditor(EditorBuilder * builder, bool folded)
			{
				builder->addTitle("Light");

				char rgb[10];
				sprintf(rgb, "%02X%02X%02X", (int)(color.r * 255), (int)(color.g * 255), (int)(color.b * 255));

				builder->beginGroup("Color");
				/*builder->addTextBox(rgb, [this](const std::string &newValue) {
					std::stringstream c(newValue);
					unsigned int rgb;
					c >> std::hex >> rgb;
					color.b = ((rgb >> 0) & 255) / 255.0f;
					color.g = ((rgb >> 8) & 255) / 255.0f;
					color.r = ((rgb >> 16) & 255) / 255.0f;
				});*/

				builder->addColorBox(color, [this](const glm::vec4 &newColor) {this->color = newColor; rebake(); });

				builder->endGroup();

				builder->beginGroup("Intensity");
				builder->addFloatBox(intensity, 0, 180, [this](float newValue) { intensity = newValue; rebake(); });
				builder->endGroup();

				builder->beginGroup("Light type");
				builder->addComboBox(type == Type::directional ? "Directional" :
									(type == Type::point ? "Point" :
									(type == Type::spot ? "Spot" :
										"none")), { "Directional", "Point", "Spot" }, [this, builder](const std::string &newValue) {
					if (newValue == "Directional")
						type = Type::directional;
					else if (newValue == "Point")
						type = Type::point;
					else if (newValue == "Spot")
						type = Type::spot;
					rebake();
					builder->updateComponentsPanel();
				});
				builder->endGroup();

				builder->beginGroup("Spotlight Angle");
				builder->addFloatBox(spotlightAngle, 0, 180, [this](float newValue) { spotlightAngle = newValue; rebake(); });
				builder->endGroup();

				builder->beginGroup("Range");
				builder->addFloatBox(range, 0, 100, [this](float newValue) { range = newValue; rebake(); });
				builder->endGroup();

				builder->beginGroup("Ambient");
				builder->addFloatBox(directionalAmbient, 0, 100, [this](float newValue) { directionalAmbient  = newValue; rebake(); });
				builder->endGroup();

				builder->beginGroup("Cutoff");
				builder->addFloatBox(cutoff, 0, 1, [this](float newValue) { cutoff = newValue; rebake(); });
				builder->endGroup();

				builder->beginGroup("Baking");
				builder->addComboBox(baking == Baking::realtime ? "Realtime" : "Baked", { "Realtime", "Baked" }, [this](const std::string &newValue) {
					baking = newValue == "Realtime" ? Baking::realtime : Baking::baked;
					rebake();
				});
				builder->endGroup();

				builder->beginGroup("Shadow");
				builder->addComboBox(shadow == Shadow::none ? "None" :
									(shadow == Shadow::shadowmap ? "Shadowmap" :
									(shadow == Shadow::shadowvolume ? "Shadowvolume" :
										"None")), { "None", "Shadowmap", "Shadowvolume" }, [this](const std::string &newValue) {
					if (newValue == "None")
						shadow = Shadow::none;
					else if (newValue == "Shadowmap")
						shadow = Shadow::shadowmap;
					else if (newValue == "Shadowvolume")
						shadow = Shadow::shadowvolume;
					rebake();
				});				
				builder->endGroup();
			}

			bool Light::inFrustum(vrlib::math::Frustum * frustum)
			{
				if (type == Type::directional || type == Type::spot)
					return true; //TODO: spotlight

				glm::vec3 position = node->transform->getGlobalPosition();
				glm::vec3 camerapos(glm::inverse(frustum->modelviewMatrix) * glm::vec4(0, 0, 0, 1));
				float rrange = realRange();
				//TODO: occlusion test?
				if (glm::distance(camerapos, position) < rrange)
					return true;

				if (glm::distance(glm::vec3(glm::inverse(frustum->modelviewMatrix) * glm::vec4(0, 0, 0, 1)), position) > 30) 
					return false;

				if (frustum->sphereInFrustum(position, rrange * 0.1f))
					return true;
				return false;
			}

			float Light::realRange()
			{
				//formula from http://ogldev.atspace.co.uk/www/tutorial36/tutorial36.html and https://imdoingitwrong.wordpress.com/2011/01/31/light-attenuation/
				float kC = 1;
				float kL = 2.0f / range;
				float kQ = 1.0f / (range * range);
				float maxChannel = glm::max(glm::max(color.r, color.g), color.b);
				float adjustedRange = (-kL + glm::sqrt(kL * kL - 4 * kQ * (kC - 128.0f * maxChannel * intensity))) / (2 * kQ);
				return adjustedRange;
			}

		}
	}
}