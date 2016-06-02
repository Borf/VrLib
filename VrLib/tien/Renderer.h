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
	namespace gl { class FBO; }
	namespace tien
	{
		class Renderer : public Node
		{
			bool treeDirty;
			virtual void setTreeDirty() override;

			std::list<Node*> renderables;
			std::list<Node*> lights;
			void updateRenderables();

			enum class RenderUniform
			{
				modelMatrix,
				projectionMatrix,
				viewMatrix,
				normalMatrix,
				s_texture,
				diffuseColor,
				textureFactor,
			};
			vrlib::gl::Shader<RenderUniform>* renderShader;
			
			enum class PostLightingUniform
			{
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

			vrlib::gl::VBO<gl::VertexP3>* overlayVerts;
			vrlib::gl::VAO<gl::VertexP3>* overlayVao;

			Node* cameraNode;

			vrlib::PositionalDevice mHead;

		public:
			Renderer();

			virtual void init();
			virtual void render(const glm::mat4 &projectionMatrix, const glm::mat4 &modelViewMatrix);
			virtual void update(float elapsedTime);
		};
	}
}
