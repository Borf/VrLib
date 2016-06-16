#pragma once

#include <glm/glm.hpp>
#include <VrLib/Device.h>
#include <VrLib/gl/shader.h>
#include <VrLib/gl/VBO.h>
#include <VrLib/gl/VAO.h>
#include <VrLib/gl/Vertex.h>

#include "Node.h"

namespace vrlib
{
	class Model;
	class Texture;
	namespace gl { class FBO; }
	namespace tien
	{
		class Scene;
		class Renderer
		{
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
			
			enum class PostLightingUniform
			{
				windowSize,
				modelViewMatrix,
				projectionMatrix,
				modelViewMatrixInv,
				projectionMatrixInv,
				lightType,
				lightPosition,
				lightDirection,
				lightColor,
				lightRange,
				s_color,
				s_normal,
				s_depth,
			};
			vrlib::gl::Shader<PostLightingUniform>* postLightingShader;
			vrlib::gl::FBO* gbuffers;
			vrlib::Texture* defaultNormalMap;

			vrlib::gl::VBO<gl::VertexP3>* overlayVerts;
			vrlib::gl::VAO<gl::VertexP3>* overlayVao;
			glm::ivec2 sphere;
			glm::ivec2 cone;


			vrlib::PositionalDevice mHead;


			enum class SkydomeUniforms
			{
				modelViewMatrix,
				projectionMatrix,
				glow,
				color,
				sunDirection,
			};
			vrlib::gl::Shader<SkydomeUniforms>* skydomeShader;
			vrlib::Model* skydome;
			vrlib::Texture* skydomeColor;
			vrlib::Texture* skydomeGlow;

			enum class BillboardUniforms
			{
				projectionMatrix,
				mat,
				s_texture
			};
			vrlib::gl::Shader<BillboardUniforms>* billboardShader;
			vrlib::Model* sun;
			vrlib::Model* moon;


		public:
			Renderer();

			virtual void init();

			void buildOverlay();

			virtual void render(const Scene& scene, const glm::mat4 &projectionMatrix, const glm::mat4 &modelViewMatrix);
		};
	}
}
