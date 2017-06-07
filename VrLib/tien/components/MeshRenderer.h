#pragma once

#include "Renderable.h"
#include <string>
#include <map>

#include <VrLib/json.hpp>
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
			public:
				class Mesh;
			private:
				class ModelDeferredRenderContext : public Renderable::RenderContext, public Singleton<ModelDeferredRenderContext>
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

				class ModelForwardRenderContext : public Renderable::RenderContext, public MapSingleton<ModelForwardRenderContext, std::string>
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
						s_specularmap,
						diffuseColor,
						textureFactor,
						shinyness,
					};
					std::string shaderFile;
					ModelForwardRenderContext(const std::string &shader);
					//ModelForwardRenderContext();
					vrlib::gl::Shader<RenderUniform>* renderShader;
					vrlib::Texture* defaultNormalMap;
					virtual void init() override;
					virtual void frameSetup(const glm::mat4 &projectionMatrix, const glm::mat4 &viewMatrix) override;
				};
				class ModelShadowRenderContext : public Renderable::RenderContext, public Singleton<ModelShadowRenderContext>
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
				gl::VAO* vao;

				Mesh* prevMesh = nullptr;

			public:
				class Mesh : public CollisionMesh
				{
				public:
					Mesh();
					Mesh(const json &data);
					std::string guid;

					vrlib::Material material;
					std::vector<vrlib::gl::VertexP3N2B2T2T2> vertices;
					std::vector<unsigned int> indices;

					json toJson();

					virtual void collisionFractions(const vrlib::math::Ray & ray, std::function<bool(float)> callback) override;
				};

				class Cube : public Mesh { public:			Cube(); };
				class Plane : public Mesh { public:			Plane(); };
				class Sphere : public Mesh { public:		Sphere(); };

				MeshRenderer(Mesh* mesh = nullptr);
				MeshRenderer(const json &data, const json &totalJson);
				~MeshRenderer();

				Mesh* mesh;

				json toJson(json &meshes) const override;
				virtual void buildEditor(EditorBuilder* builder, bool folded) override;
				void updateMesh();

				virtual void update(float elapsedTime, Scene& scene) override;

				void drawDeferredPass() override;
				void drawForwardPass() override;
				void drawShadowMap() override;

				bool castShadow;
				bool cullBackFaces = true;
				bool useDeferred = true;
			};
		}
	}
}