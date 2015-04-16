#pragma once

#include <VrLib/Viewport.h>
#include <glm/glm.hpp>
namespace vrlib
{
	class ProjectionViewport : public Viewport
	{

		glm::vec3 screenTopLeft;
		glm::vec3 screenTopRight;
		glm::vec3 screenBottomLeft;
		glm::vec3 screenBottomRight;
		int eye;

	public:
		ProjectionViewport(User* user, int eye, glm::vec3 topleft, glm::vec3 topright, glm::vec3 bottomleft, glm::vec3 bottomright);


		virtual void draw(Application* application);
		virtual glm::mat4 getProjectionMatrix();
	};
}