#pragma once

#include <VrLib/Viewport.h>
#include <VrLib/gl/shader.h>
#include <glm/glm.hpp>


namespace vrlib
{
	class PositionalDevice;
	class Model;

	class SimulatorViewport : public Viewport
	{
		class ViewPortCorners
		{
		public:
			glm::vec3 tl;
			glm::vec3 tr;
			glm::vec3 bl;
			glm::vec3 br;
			ViewPortCorners(glm::vec3 topleft, glm::vec3 topright, glm::vec3 bottomleft, glm::vec3 bottomright)
			{
				tl = topleft;
				tr = topright;
				bl = bottomleft;
				br = bottomright;
			}
		};
		std::list<ViewPortCorners> viewports;
		PositionalDevice* cameraDevice;
		int windowWidth;
		int windowHeight;

		Model* faceModel;
		enum class Uniforms
		{
			projectionMatrix,
	//		modelMatrix,
			viewMatrix,
		};
		gl::Shader<Uniforms>* shader;

	public:
		SimulatorViewport(User* user, PositionalDevice* cameraDevice);

		void addViewPort(glm::vec3 topleft, glm::vec3 topright, glm::vec3 bottomleft, glm::vec3 bottomright);

		virtual void draw(Application* application);
		virtual glm::mat4 getProjectionMatrix();
		void setWindowSize(int windowWidth, int windowHeight);
	};
}