#include <VrLib/Viewports/ProjectionViewport.h>
#include <VrLib/User.h>
#include <VrLib/Application.h>

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace vrlib
{
	ProjectionViewport::ProjectionViewport(User* user, int eye, glm::vec3 topleft, glm::vec3 topright, glm::vec3 bottomleft, glm::vec3 bottomright) : Viewport(user)
	{
		this->eye = eye;
		screenTopLeft = topleft;
		screenTopRight = topright;
		screenBottomLeft = bottomleft;
		screenBottomRight = bottomright;
	}

	glm::mat4 ProjectionViewport::getProjectionMatrix()
	{ //from http://csc.lsu.edu/~kooima/pdfs/gen-perspective.pdf
		glm::vec3 pe = user->getEyePosition(eye);

		float nearPlane = 0.001f;
		float farPlane = 1000;

		/*	glm::vec3 pa = glm::vec3(glm::vec4(screenBottomLeft,0) * user->matrix);
			glm::vec3 pb = glm::vec3(glm::vec4(screenBottomRight,0) * user->matrix);
			glm::vec3 pc = glm::vec3(glm::vec4(screenTopLeft,0) * user->matrix);*/

		glm::vec3 pa = screenBottomLeft;
		glm::vec3 pb = screenBottomRight;
		glm::vec3 pc = screenTopLeft;


		glm::vec3 vr = pb - pa;
		glm::vec3 vu = pc - pa;

		vr = glm::normalize(vr);
		vu = glm::normalize(vu);

		glm::vec3 vn = glm::cross(vr, vu);
		vn = glm::normalize(vn);

		glm::vec3 va = pa - pe;
		glm::vec3 vb = pb - pe;
		glm::vec3 vc = pc - pe;

		float d = -glm::dot(va, vn);

		float l = glm::dot(vr, va) * nearPlane / d;
		float r = glm::dot(vr, vb) * nearPlane / d;
		float b = glm::dot(vu, va) * nearPlane / d;
		float t = glm::dot(vu, vc) * nearPlane / d;

		glm::mat4 ret = glm::frustum(l, r, b, t, nearPlane, farPlane);

		glm::mat4 rot(
			vr[0], vu[0], vn[0], 0,
			vr[1], vu[1], vn[1], 0,
			vr[2], vu[2], vn[2], 0,
			0, 0, 0, 1
			);

		return glm::translate(ret * rot, -pe);;
	}

	void ProjectionViewport::draw(Application* application)
	{
		glm::mat4 projectionMatrix = getProjectionMatrix();
		glEnable(GL_DEPTH_TEST);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glLoadMatrixf(glm::value_ptr(projectionMatrix));
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();


		application->draw(projectionMatrix, glm::mat4(), user->matrix);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);

		glUseProgram(0);
		glEnable(GL_DEPTH_TEST);
		glLoadIdentity();

		glColor4f(1, 1, 1, 1);

	}

}