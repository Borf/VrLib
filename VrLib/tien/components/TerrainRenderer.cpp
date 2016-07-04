#include "TerrainRenderer.h"
#include "Transform.h"
#include "../Terrain.h"
#include "../Node.h"
#include <vector>

#include <VrLib/Texture.h>

namespace vrlib
{
	namespace tien
	{
		namespace components
		{
			TerrainRenderer::TerrainRenderer(Terrain* _terrain) : terrain(*_terrain)
			{
				smoothNormals = true;
				renderContext = TerrainRenderContext::getInstance();
				renderContextShadow = TerrainRenderShadowContext::getInstance();

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
								if(terrain.isValid(x+xx, y+yy) && x+xx < terrain.width-1 && y+yy < terrain.height-1)
									normals[x][y] += polyNormals[x+xx][y+yy];
							}
						}
						normals[x][y] = glm::normalize(normals[x][y]);
					}
				}



				std::vector<gl::VertexP3N2B2T2T2> vertices;
				for (int x = 0; x < terrain.width-1; x++)
				{
					for (int y = 0; y < terrain.height - 1; y++)
					{
						gl::VertexP3N2B2T2T2 v;
						glm::vec3 tan;

						gl::setN3(v, normals[x][y]);
						tan = glm::normalize(glm::cross(normals[x][y], glm::vec3(0, 0, 1)));
						gl::setTan3(v, tan);
						gl::setBiTan3(v, glm::normalize(glm::cross(tan, normals[x][y])));
						gl::setP3(v, glm::vec3(x, terrain[x][y], y));
 						gl::setT2(v, glm::vec2(x, y)/10.0f);
						vertices.push_back(v);

						gl::setN3(v, normals[x][y+1]);
						tan = glm::normalize(glm::cross(normals[x][y+1], glm::vec3(0, 0, 1)));
						gl::setTan3(v, tan);
						gl::setBiTan3(v, glm::normalize(glm::cross(normals[x][y+1], tan)));
						gl::setP3(v, glm::vec3(x, terrain[x][y+1], y + 1));
						gl::setT2(v, glm::vec2(x, y + 1) / 10.0f);
						vertices.push_back(v);

						gl::setN3(v, normals[x+1][y+1]);
						tan = glm::normalize(glm::cross(normals[x+1][y+1], glm::vec3(0, 0, 1)));
						gl::setTan3(v, tan);
						gl::setBiTan3(v, glm::normalize(glm::cross(normals[x+1][y+1], tan)));
						gl::setP3(v, glm::vec3(x+1, terrain[x+1][y+1], y+1));
						gl::setT2(v, glm::vec2(x+1, y+1) / 10.0f);
						vertices.push_back(v);

						gl::setN3(v, normals[x+1][y]);
						tan = glm::normalize(glm::cross(normals[x+1][y], glm::vec3(0, 0, 1)));
						gl::setTan3(v, tan);
						gl::setBiTan3(v, glm::normalize(glm::cross(normals[x+1][y], tan)));
						gl::setP3(v, glm::vec3(x + 1, terrain[x+1][y], y));
						gl::setT2(v, glm::vec2(x + 1, y) / 10.0f);
						vertices.push_back(v);

					}
				}
				vbo.bind();
				vbo.setData(vertices.size(), &vertices[0], GL_STATIC_DRAW);
				vao = new gl::VAO<gl::VertexP3N2B2T2T2>(&vbo);
			}



			void TerrainRenderer::addMaterialLayer(vrlib::Texture* diffuse, vrlib::Texture* normal, vrlib::Texture* mask)
			{
				materials.push_back({ diffuse, normal, mask });
			}
			void TerrainRenderer::addMaterialLayer(const std::string &diffuse, const std::string &normal, const std::string &mask)
			{
				materials.push_back({ vrlib::Texture::loadCached(diffuse), vrlib::Texture::loadCached(normal), vrlib::Texture::loadCached(mask)  });
			}


			void TerrainRenderer::draw()
			{
				components::Transform* t = node->getComponent<Transform>();

				TerrainRenderContext* context = dynamic_cast<TerrainRenderContext*>(renderContext);
				context->renderShader->use();
				context->renderShader->setUniform(TerrainRenderContext::RenderUniform::modelMatrix, t->globalTransform);
				context->renderShader->setUniform(TerrainRenderContext::RenderUniform::normalMatrix, glm::transpose(glm::inverse(glm::mat3(t->globalTransform))));
				context->renderShader->setUniform(TerrainRenderContext::RenderUniform::heightmapSize, glm::vec2(terrain.width, terrain.height)); //TODO: only do this once


				vao->bind();
				glDisable(GL_BLEND);
				glActiveTexture(GL_TEXTURE0);
				context->black->bind();
				glActiveTexture(GL_TEXTURE1);
				context->black->bind();
				glActiveTexture(GL_TEXTURE2);
				context->white->bind();

				glDrawArrays(GL_QUADS, 0, terrain.width * terrain.height * 4);

				glEnablei(GL_BLEND, 0);
				glEnablei(GL_BLEND, 1);
				glBlendFunci(0, GL_ONE, GL_ONE);
				glBlendFunci(1, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glDepthFunc(GL_EQUAL);

				for (size_t i = 0; i < materials.size(); i++)
				{
					glActiveTexture(GL_TEXTURE0);
					materials[i].diffuse->bind();
					glActiveTexture(GL_TEXTURE1);
					materials[i].normal->bind();
					glActiveTexture(GL_TEXTURE2);
					materials[i].mask->bind();
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
				context->renderShader->setUniform(TerrainRenderShadowContext::RenderUniform::modelMatrix, t->globalTransform);

				vao->bind();
				glDrawArrays(GL_QUADS, 0, terrain.width * terrain.height * 4);
				vao->unBind();
			}








			void TerrainRenderer::TerrainRenderContext::init()
			{
				renderShader = new vrlib::gl::Shader<RenderUniform>("data/vrlib/tien/shaders/terrain.vert", "data/vrlib/tien/shaders/terrain.frag");
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

				defaultNormalMap = vrlib::Texture::loadCached("data/vrlib/tien/textures/emptyNormalMap.png");
				black = vrlib::Texture::loadCached("data/vrlib/tien/textures/black.png");
				white = vrlib::Texture::loadCached("data/vrlib/tien/textures/white.png");
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
				renderShader = new vrlib::gl::Shader<RenderUniform>("data/vrlib/tien/shaders/defaultShadow.vert", "data/vrlib/tien/shaders/defaultShadow.frag");
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