#include "TerrainRenderer.h"
#include "Transform.h"
#include "../Terrain.h"
#include "../Node.h"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <VrLib/json.hpp>
#include <VrLib/Texture.h>
#include <VrLib/Image.h>

namespace vrlib
{
	namespace tien
	{
		namespace components
		{
			TerrainRenderer::TerrainRenderer(Terrain* _terrain) : terrain(*_terrain)
			{
				smoothNormals = true;
				renderContextDeferred = TerrainRenderContext::getInstance();
				renderContextShadow = TerrainRenderShadowContext::getInstance();

				rebuildBuffers();
			}


			json TerrainRenderer::toJson(json &meshes) const
			{
				json ret;
				ret["type"] = "terrainrenderer";
				return ret;
			}

			void TerrainRenderer::rebuildBuffers()
			{

				std::vector<std::vector<glm::vec3>> polyNormals;
				polyNormals.resize(terrain.width - 1, std::vector<glm::vec3>(terrain.height - 1, glm::vec3(0, 1, 0)));
				std::vector<std::vector<glm::vec3>> normals;
				normals.resize(terrain.width, std::vector<glm::vec3>(terrain.height, glm::vec3(0, 0, 0)));

				for (int x = 0; x < terrain.width - 1; x++)
					for (int y = 0; y < terrain.height - 1; y++)
						polyNormals[x][y] = glm::normalize(glm::cross(
							glm::vec3(0, terrain[x][y + 1] - terrain[x][y], 1),
							glm::vec3(1, terrain[x + 1][y] - terrain[x][y], 0)
							));

				for (int x = 0; x < terrain.width; x++)
				{
					for (int y = 0; y < terrain.height; y++)
					{
						for (int xx = -1; xx <= 0; xx++)
						{
							for (int yy = -1; yy <= 0; yy++)
							{
								if (terrain.isValid(x + xx, y + yy) && x + xx < terrain.width - 1 && y + yy < terrain.height - 1)
									normals[x][y] += polyNormals[x + xx][y + yy];
							}
						}
						normals[x][y] = glm::normalize(normals[x][y]);
					}
				}



				std::vector<gl::VertexP3N2B2T2T2> vertices;
				for (int x = 0; x < terrain.width - 1; x++)
				{
					for (int y = 0; y < terrain.height - 1; y++)
					{
						gl::VertexP3N2B2T2T2 v;
						glm::vec3 tan;

						gl::setN3(v, normals[x][y]);
						tan = glm::normalize(glm::cross(normals[x][y], glm::vec3(1, 0, 0)));
						gl::setTan3(v, tan);
						gl::setBiTan3(v, glm::normalize(glm::cross(normals[x][y], tan)));
						gl::setP3(v, glm::vec3(x, terrain[x][y], y));
						gl::setT2(v, glm::vec2(x, y) / 10.0f);
						vertices.push_back(v);

						gl::setN3(v, normals[x][y + 1]);
						tan = glm::normalize(glm::cross(normals[x][y + 1], glm::vec3(1, 0, 0)));
						gl::setTan3(v, tan);
						gl::setBiTan3(v, glm::normalize(glm::cross(normals[x][y + 1], tan)));
						gl::setP3(v, glm::vec3(x, terrain[x][y + 1], y + 1));
						gl::setT2(v, glm::vec2(x, y + 1) / 10.0f);
						vertices.push_back(v);

						gl::setN3(v, normals[x + 1][y + 1]);
						tan = glm::normalize(glm::cross(normals[x + 1][y + 1], glm::vec3(1, 0, 0)));
						gl::setTan3(v, tan);
						gl::setBiTan3(v, glm::normalize(glm::cross(normals[x + 1][y + 1], tan)));
						gl::setP3(v, glm::vec3(x + 1, terrain[x + 1][y + 1], y + 1));
						gl::setT2(v, glm::vec2(x + 1, y + 1) / 10.0f);
						vertices.push_back(v);

						gl::setN3(v, normals[x + 1][y]);
						tan = glm::normalize(glm::cross(normals[x + 1][y], glm::vec3(1, 0, 0)));
						gl::setTan3(v, tan);
						gl::setBiTan3(v, glm::normalize(glm::cross(normals[x + 1][y], tan)));
						gl::setP3(v, glm::vec3(x + 1, terrain[x + 1][y], y));
						gl::setT2(v, glm::vec2(x + 1, y) / 10.0f);
						vertices.push_back(v);

					}
				}
				vbo.bind();
				vbo.setData(vertices.size(), &vertices[0], GL_STATIC_DRAW);
				vao = new gl::VAO(&vbo);
			}




			void TerrainRenderer::addMaterialLayer(vrlib::Texture* diffuse, vrlib::Texture* normal, vrlib::Texture* mask)
			{
				materials.push_back({ diffuse, normal, mask });
			}
			void TerrainRenderer::addMaterialLayer(const std::string &diffuse, const std::string &normal, const std::string &mask)
			{
				materials.push_back({ vrlib::Texture::loadCached(diffuse), vrlib::Texture::loadCached(normal), vrlib::Texture::loadCached(mask)  });
			}
			void TerrainRenderer::addMaterialLayer(const std::string &diffuse, const std::string &normal, float minHeight, float maxHeight, float fadeDist)
			{
				vrlib::Image* mask = new vrlib::Image(terrain.width, terrain.height);

				for (int x = 0; x < terrain.width; x++)
				{
					for (int y = 0; y < terrain.height; y++)
					{
						if (terrain[x][y] >= minHeight && terrain[x][y] <= maxHeight)
							(*mask)[x][y][0] = 255;
						else
							(*mask)[x][y][0] = (unsigned char)glm::floor(255 * glm::max(0.0f, 1.0f - glm::min(glm::abs(terrain[x][y] - minHeight), glm::abs(terrain[x][y] - maxHeight)) / fadeDist));
					}
				}


				materials.push_back({ vrlib::Texture::loadCached(diffuse), vrlib::Texture::loadCached(normal), new vrlib::Texture(mask) });
			}

			void TerrainRenderer::setTerrainMask(vrlib::Texture* mask)
			{
				TerrainRenderContext* context = dynamic_cast<TerrainRenderContext*>(renderContextDeferred);
				this->terrainMask = mask;
				context->terrainMask = mask;
			}


			void TerrainRenderer::drawDeferredPass()
			{
				components::Transform* t = node->getComponent<Transform>();

				TerrainRenderContext* context = dynamic_cast<TerrainRenderContext*>(renderContextDeferred);
				context->renderShader->use();
				context->renderShader->setUniform(TerrainRenderContext::RenderUniform::modelMatrix, t->globalTransform);
				context->renderShader->setUniform(TerrainRenderContext::RenderUniform::normalMatrix, glm::transpose(glm::inverse(glm::mat3(t->globalTransform))));
				context->renderShader->setUniform(TerrainRenderContext::RenderUniform::heightmapSize, glm::vec2(terrain.width, terrain.height)); //TODO: only do this once
				if (terrainMask)
					context->terrainMask = terrainMask;

				vao->bind();
				glDisable(GL_BLEND);
				glActiveTexture(GL_TEXTURE0);
				context->black->bind();
				glActiveTexture(GL_TEXTURE1);
				context->black->bind();
				glActiveTexture(GL_TEXTURE2);
				context->terrainMask->bind();

				glDrawArrays(GL_QUADS, 0, terrain.width * terrain.height * 4);

				glEnablei(GL_BLEND, 0);
				glEnablei(GL_BLEND, 1);
				glBlendFunci(0, GL_ONE, GL_ONE);
				glBlendFunci(1, GL_ONE, GL_ZERO);
				glDepthFunc(GL_EQUAL);

				for (size_t i = 0; i < materials.size(); i++)
				{
					glActiveTexture(GL_TEXTURE0);
					if(materials[i].diffuse)
						materials[i].diffuse->bind();
					else
						context->white->bind();
					glActiveTexture(GL_TEXTURE1);
					if(materials[i].normal)
						materials[i].normal->bind();
					else
						context->defaultNormalMap->bind();
					glActiveTexture(GL_TEXTURE2);
					materials[i].mask->bind();
					glDrawArrays(GL_QUADS, 0, terrain.width * terrain.height * 4);
				}
				if (materials.size() == 0)
				{
					glActiveTexture(GL_TEXTURE0);
					context->white->bind();
					glActiveTexture(GL_TEXTURE1);
					context->defaultNormalMap->bind();
					glActiveTexture(GL_TEXTURE2);
					context->white->bind();
					glDrawArrays(GL_QUADS, 0, terrain.width * terrain.height * 4);
				}
				


				glActiveTexture(GL_TEXTURE0);
				glDepthFunc(GL_LEQUAL);
				glDisable(GL_BLEND);

			}



			void TerrainRenderer::drawShadowMap()
			{
				components::Transform* t = node->getComponent<Transform>();

				TerrainRenderShadowContext* context = dynamic_cast<TerrainRenderShadowContext*>(renderContextShadow);
				context->renderShader->use();
				context->renderShader->setUniform(TerrainRenderShadowContext::RenderUniform::modelMatrix, glm::translate(t->globalTransform, glm::vec3(0,0, 0)));

				vao->bind();
				glDrawArrays(GL_QUADS, 0, terrain.width * terrain.height * 4);
				vao->unBind();
			}








			void TerrainRenderer::TerrainRenderContext::init()
			{
				renderShader = new vrlib::gl::Shader<RenderUniform>("data/vrlib/tien/shaders/TerrainRenderer.deferred.vert", "data/vrlib/tien/shaders/TerrainRenderer.deferred.frag");
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
				renderShader->registerUniform(RenderUniform::s_mask, "s_mask");
				renderShader->registerUniform(RenderUniform::diffuseColor, "diffuseColor");
				renderShader->registerUniform(RenderUniform::heightmapSize, "heightmapSize");
				renderShader->use();
				renderShader->setUniform(RenderUniform::s_texture, 0);
				renderShader->setUniform(RenderUniform::s_normalmap, 1);
				renderShader->setUniform(RenderUniform::s_mask, 2);

				defaultNormalMap = vrlib::Texture::loadCached("data/vrlib/tien/textures/defaultNormalMap.png");
				black = vrlib::Texture::loadCached("data/vrlib/tien/textures/black.png");
				white = vrlib::Texture::loadCached("data/vrlib/tien/textures/white.png");
				terrainMask = vrlib::Texture::loadCached("data/vrlib/tien/textures/white.png");
			}
			void TerrainRenderer::TerrainRenderContext::frameSetup(const glm::mat4 & projectionMatrix, const glm::mat4 & viewMatrix)
			{
				renderShader->use();
				renderShader->setUniform(RenderUniform::projectionMatrix, projectionMatrix);
				renderShader->setUniform(RenderUniform::viewMatrix, viewMatrix);
				renderShader->setUniform(RenderUniform::diffuseColor, glm::vec4(1, 1, 1, 1));
			}






			 
			void TerrainRenderer::TerrainRenderShadowContext::frameSetup(const glm::mat4 & projectionMatrix, const glm::mat4 & viewMatrix)
			{
				renderShader->use();
				renderShader->setUniform(RenderUniform::projectionMatrix, projectionMatrix);
				renderShader->setUniform(RenderUniform::viewMatrix, viewMatrix);
			}
			void TerrainRenderer::TerrainRenderShadowContext::init()
			{
				renderShader = new vrlib::gl::Shader<RenderUniform>("data/vrlib/tien/shaders/TerrainRenderer.shadow.vert", "data/vrlib/tien/shaders/TerrainRenderer.shadow.frag");
				renderShader->bindAttributeLocation("a_position", 0);
				renderShader->link();
				//shader->bindFragLocation("fragColor", 0);
				renderShader->registerUniform(RenderUniform::modelMatrix, "modelMatrix");
				renderShader->registerUniform(RenderUniform::projectionMatrix, "projectionMatrix");
				renderShader->registerUniform(RenderUniform::viewMatrix, "viewMatrix");
				renderShader->use();
			}



		}
	}
}