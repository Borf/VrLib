#pragma once

#include <glm/glm.hpp>
#include <VrLib/gl/shader.h>

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
				lightPosition,
				lightColor,
				lightRange,
				s_color,
				s_normal,
				s_position
			};
			vrlib::gl::Shader<PostLightingUniform>* postLightingShader;
			vrlib::gl::FBO* gbuffers;
			Node* cameraNode;
		public:
			Renderer();

			virtual void init();
			virtual void render(const glm::mat4 &projectionMatrix, const glm::mat4 &modelViewMatrix);
			virtual void update(float elapsedTime);
		};
	}
}
