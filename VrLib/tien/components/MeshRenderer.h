#pragma once

#include "Renderable.h"
#include <string>
#include <map>

#include <VrLib/Model.h>
#include <VrLib/gl/VIO.h>
#include <VrLib/gl/VAO.h>
#include <VrLib/gl/VBO.h>

namespace vrlib
{
	class Texture;

	namespace tien
	{
		namespace components
		{
			class MeshRenderer : public Renderable
			{
			private:
				class ModelRenderContext : public Renderable::RenderContext, public Singleton<ModelRenderContext>
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
						diffuseColor,
						textureFactor,
					};
					vrlib::gl::Shader<RenderUniform>* renderShader;
					vrlib::Texture* defaultNormalMap;
					virtual void init() override;
					virtual void frameSetup(const glm::mat4 &projectionMatrix, const glm::mat4 &viewMatrix) override;
				};
				class ModelRenderShadowContext : public Renderable::RenderContext, public Singleton<ModelRenderShadowContext>
				{
				public:
					enum class RenderUniform
					{
						modelMatrix,
						projectionMatrix,
						viewMatrix,
						outputPosition
					};
					vrlib::gl::Shader<RenderUniform>* renderShader;
					virtual void init() override;
					virtual void frameSetup(const glm::mat4 &projectionMatrix, const glm::mat4 &viewMatrix) override;
					virtual void useCubemap(bool) override;
				};
				gl::VBO<gl::VertexP3N2B2T2T2> vbo;
				gl::VIO<unsigned int> vio;
				gl::VAO<gl::VertexP3N2B2T2T2>* vao;


			public:
				class Mesh
				{
				public:
					vrlib::Material material;
					std::vector<vrlib::gl::VertexP3N2B2T2T2> vertices;
					std::vector<unsigned int> indices;
				};


				MeshRenderer(Mesh* mesh = nullptr);
				~MeshRenderer();

				Mesh* mesh;

				void updateMesh();


				void draw() override;
				void drawShadowMap() override;

				bool castShadow;
			};
		}
	}
}