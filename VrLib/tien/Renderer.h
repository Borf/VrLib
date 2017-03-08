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
			enum class PostLightingUniform
			{
				windowSize,
				windowPos,
				modelViewMatrix,
				projectionMatrix,
				modelViewMatrixInv,
				projectionMatrixInv,
				shadowMatrix,
				lightType,
				lightPosition,
				lightDirection,
				lightColor,
				lightIntensity, 
				lightRange,
				lightCutoff,
				lightCastShadow,
				lightAmbient,
				lightSpotAngle,
				cameraPosition,
				s_color,
				s_normal,
				s_depth,
				s_shadowmap,
				s_shadowmapcube,
				debug,
			};
			vrlib::gl::Shader<PostLightingUniform>* postLightingShader;

			enum class PostLightingStencilUniform
			{
				lightType,
				projectionMatrix,
				modelViewMatrix,
			};
			vrlib::gl::Shader<PostLightingStencilUniform>* postLightingStencilShader;



			enum class PhysicsDebugUniform
			{
				projectionMatrix,
				modelViewMatrix,
			};
			vrlib::gl::Shader<PhysicsDebugUniform>* physicsDebugShader;


			enum class SimpleDebugUniform
			{
				projectionMatrix,
				modelViewMatrix,
				s_texture,
				textureFactor,
				color,
				showAlpha,
			};
			vrlib::gl::Shader<SimpleDebugUniform>* simpleDebugShader;




			vrlib::gl::FBO* gbuffers;

			vrlib::gl::VBO<gl::VertexP3>* overlayVerts;
			vrlib::gl::VAO* overlayVao;
			glm::ivec2 sphere;
			glm::ivec2 cone;

			GLuint fakeDepthBuffer = -1;

			vrlib::PositionalDevice mHead;


		public:
			Renderer();

			virtual void init();

			void buildOverlay();

			virtual void render(const Scene& scene, const glm::mat4 &projectionMatrix, const glm::mat4 &modelViewMatrix, Node* cameraNode, int renderId = 0);


			bool drawPhysicsDebug;
			bool drawLightDebug;

			enum class DrawMode
			{
				Default,
				Albedo,
				Normals,
				Specular,
				Lightmaps,
			} drawMode;
			int debugLightMapIndex = 0;

			static int drawCalls;

		};
	}
}
