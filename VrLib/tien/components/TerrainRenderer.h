#pragma once

#include "Renderable.h"

#include <VrLib/gl/VIO.h>
#include <VrLib/gl/VBO.h>
#include <VrLib/gl/VAO.h>
#include <VrLib/gl/Vertex.h>

namespace vrlib
{
	class Texture;

	namespace tien
	{
		class Terrain;
		namespace components
		{
			class TerrainRenderer : public Renderable
			{
				class TerrainRenderContext : public Renderable::RenderContext, public Singleton<TerrainRenderContext>
				{
				public:
					enum class RenderUniform
					{
						modelMatrix,
						projectionMatrix,
						viewMatrix,
						normalMatrix,
						s_texture,
						s_normalmap,
						s_mask,
						diffuseColor,
						heightmapSize
					};
					vrlib::gl::Shader<RenderUniform>* renderShader;
					virtual void init() override;
					virtual void frameSetup(const glm::mat4 &projectionMatrix, const glm::mat4 &viewMatrix) override;

					vrlib::Texture* black;
					vrlib::Texture* terrainMask;
					vrlib::Texture* white;
					vrlib::Texture* defaultNormalMap;
				};
				class TerrainRenderShadowContext : public Renderable::RenderContext, public Singleton<TerrainRenderShadowContext>
				{
				public:
					enum class RenderUniform
					{
						modelMatrix,
						projectionMatrix,
						viewMatrix,
					};
					vrlib::gl::Shader<RenderUniform>* renderShader;
					virtual void init() override;
					virtual void frameSetup(const glm::mat4 &projectionMatrix, const glm::mat4 &viewMatrix) override;
				};

				gl::VBO<gl::VertexP3N2B2T2T2> vbo;
				gl::VIO<unsigned int> vio;
				vrlib::Texture* terrainMask = nullptr;

			public:
				gl::VAO* vao;

				class Material
				{
				public:
					vrlib::Texture* diffuse;
					vrlib::Texture* normal;
					vrlib::Texture* mask;
				};
				std::vector<Material> materials;

				TerrainRenderer(Terrain* terrain);
				Terrain& terrain;
				bool smoothNormals;

				void drawDeferredPass() override;
				void drawForwardPass() override {};
				void drawShadowMap() override;
				nlohmann::json toJson(nlohmann::json &meshes) const override;

				void rebuildBuffers();
				void updateBuffers(int x, int y);


				void addMaterialLayer(vrlib::Texture* diffuse, vrlib::Texture* normal, vrlib::Texture* mask);
				void addMaterialLayer(const std::string &diffuse, const std::string &normal, const std::string &mask);
				void addMaterialLayer(const std::string &diffuse, const std::string &normal, float minHeight, float maxHeight, float fadeDist);

				void setTerrainMask(vrlib::Texture* mask);

				int addRoadTexture(vrlib::Texture* diffuse, vrlib::Texture* normal); //TODO
				void addRoad(); // TODO


			};
		}
	}
}