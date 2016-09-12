#include <VrLib/Viewports/SimulatorViewport.h>
#include <VrLib/Kernel.h>
#include <VrLib/Device.h>
#include <VrLib/Application.h>
#include <VrLib/User.h>

#include <VrLib/gl/VBO.h>
#include <VrLib/gl/Vertex.h>
#include <VrLib/Model.h>

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


extern float faceVerts[23688];
extern float eyeVerts[720];

namespace vrlib
{
	SimulatorViewport::SimulatorViewport(User* user, PositionalDevice* cameraDevice) : Viewport(user)
	{
		this->cameraDevice = cameraDevice;
		this->faceModel = vrlib::Model::getModel<vrlib::gl::VertexP3N3>("data/vrlib/rendermodels/face/face.obj");

		shader = new vrlib::gl::Shader<Uniforms>("data/vrlib/SimViewShader.vert", "data/vrlib/SimViewShader.frag");
		shader->bindAttributeLocation("a_position", 0);
		shader->bindAttributeLocation("a_normal", 1);
		shader->link();
		shader->registerUniform(Uniforms::projectionMatrix, "projectionMatrix");
		shader->registerUniform(Uniforms::viewMatrix, "viewMatrix");
	//	shader->registerUniform(Uniforms::modelMatrix, "modelMatrix");

	}


	glm::mat4 SimulatorViewport::getProjectionMatrix()
	{
		float aspect = (windowWidth * this->width()) / (windowHeight * this->height());
		return glm::perspective(glm::radians(45.0f), aspect, 0.01f, 500.0f);
	}

#define HEADSIZE 0.1f

	void SimulatorViewport::draw(Application* application)
	{
		resetOpenGL();

		User* user = Kernel::getInstance()->users.front();
		glm::mat4 camera;
		if (cameraDevice && cameraDevice->isInitialized())
			camera = cameraDevice->getData();

		glm::mat4 projectionMatrix = getProjectionMatrix();
		glEnable(GL_DEPTH_TEST);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glLoadMatrixf(glm::value_ptr(projectionMatrix));
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glLoadMatrixf(glm::value_ptr(camera));
		application->draw(projectionMatrix, camera);

		resetOpenGL();

		shader->use();
		shader->setUniform(Uniforms::projectionMatrix, projectionMatrix);
		shader->setUniform(Uniforms::viewMatrix, camera * user->matrix);
		faceModel->draw([this](const glm::mat4& modelMatrix)
		{

		});

/*
		glDisable(GL_LIGHTING);
		glLoadIdentity();
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(1, 1, 1, 0.25f);
		glBegin(GL_QUADS);
		for (std::list<ViewPortCorners>::iterator it = viewports.begin(); it != viewports.end(); it++)
		{
			glVertex3fv(glm::value_ptr(it->tl));
			glVertex3fv(glm::value_ptr(it->tr));
			glVertex3fv(glm::value_ptr(it->br));
			glVertex3fv(glm::value_ptr(it->bl));
		}
		glEnd();

		glDisable(GL_DEPTH_TEST);
		glColor4f(1, 1, 1, 0.75f);
		for (std::list<ViewPortCorners>::iterator it = viewports.begin(); it != viewports.end(); it++)
		{
			glBegin(GL_LINE_LOOP);
			glVertex3fv(glm::value_ptr(it->tl));
			glVertex3fv(glm::value_ptr(it->tr));
			glVertex3fv(glm::value_ptr(it->br));
			glVertex3fv(glm::value_ptr(it->bl));
			glEnd();
		}


		glColor4f(1, 1, 1, 1);
		glDisable(GL_LIGHTING);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_DEPTH_TEST);
		*/

	}

	void SimulatorViewport::addViewPort(glm::vec3 topleft, glm::vec3 topright, glm::vec3 bottomleft, glm::vec3 bottomright)
	{
		for (std::list<ViewPortCorners>::iterator it = viewports.begin(); it != viewports.end(); it++)
			if (it->tl == topleft && it->tr == topright && it->bl == bottomleft && it->br == bottomright)
				return;


		viewports.push_back(ViewPortCorners(topleft, topright, bottomleft, bottomright));
	}

	void SimulatorViewport::setWindowSize(int windowWidth, int windowHeight)
	{
		this->windowWidth = windowWidth;
		this->windowHeight = windowHeight;
	}
}
