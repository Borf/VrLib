#include <VrLib/Viewports/SimulatorViewport.h>
#include <VrLib/Kernel.h>
#include <VrLib/Device.h>
#include <VrLib/Application.h>
#include <VrLib/User.h>

#include <VrLib/gl/VBO.h>
#include <VrLib/gl/Vertex.h>
#include <VrLib/Model.h>

#include <gl/glew.h>
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
		glm::mat4 camera = cameraDevice->getData();

		float aspect = (windowWidth * this->width()) / (windowHeight * this->height());
		return glm::perspective(glm::radians(45.0f), aspect, 0.2f, 1000.0f) * camera;
	}

#define HEADSIZE 0.1f

	void SimulatorViewport::draw(Application* application)
	{
		resetOpenGL();

		User* user = Kernel::getInstance()->users.front();
		glm::mat4 projectionMatrix = getProjectionMatrix();
		glEnable(GL_DEPTH_TEST);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glLoadMatrixf(glm::value_ptr(projectionMatrix));
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		application->draw(projectionMatrix, glm::mat4(), user->matrix);

		resetOpenGL();

		shader->use();
		shader->setUniform(Uniforms::projectionMatrix, projectionMatrix);
		shader->setUniform(Uniforms::viewMatrix, glm::scale(glm::mat4(), glm::vec3(0.1f, 0.1f, 0.1f)));
		faceModel->draw([this](const glm::mat4& modelMatrix)
		{

		});

		/*
		static gl::VBO<gl::VertexPositionNormal>* faceVBO = NULL;
		if (faceVBO == NULL)
		{
			std::vector<gl::VertexPositionNormal> verts;
			for (int i = 0; i < sizeof(faceVerts) / sizeof(float); i += 3 * 3)
			{
				glm::vec3 v1(faceVerts[i + 2], faceVerts[i + 0], faceVerts[i + 1]);
				glm::vec3 v2(faceVerts[i + 5], faceVerts[i + 3], faceVerts[i + 4]);
				glm::vec3 v3(faceVerts[i + 8], faceVerts[i + 6], faceVerts[i + 7]);
				glm::vec3 normal = glm::normalize(glm::cross(v2 - v1, v3 - v1));
				verts.push_back(gl::VertexPositionNormal(v1, normal));
				verts.push_back(gl::VertexPositionNormal(v2, normal));
				verts.push_back(gl::VertexPositionNormal(v3, normal));
			}
			faceVBO = new gl::VBO<gl::VertexPositionNormal>();
			faceVBO->setData(verts.size(), &verts[0], GL_STATIC_DRAW);
		}

		static gl::VBO<gl::VertexPositionNormal>* eyeVBO = NULL;
		if (eyeVBO == NULL)
		{
			std::vector<gl::VertexPositionNormal> verts;
			for (int i = 0; i < sizeof(eyeVerts) / sizeof(float); i += 3 * 3)
			{
				glm::vec3 v1(eyeVerts[i + 2], eyeVerts[i + 0], eyeVerts[i + 1]);
				glm::vec3 v2(eyeVerts[i + 5], eyeVerts[i + 3], eyeVerts[i + 4]);
				glm::vec3 v3(eyeVerts[i + 8], eyeVerts[i + 6], eyeVerts[i + 7]);
				glm::vec3 normal = glm::normalize(glm::cross(v2 - v1, v3 - v1));
				verts.push_back(gl::VertexPositionNormal(v1, normal));
				verts.push_back(gl::VertexPositionNormal(v2, normal));
				verts.push_back(gl::VertexPositionNormal(v3, normal));
			}
			eyeVBO = new gl::VBO<gl::VertexPositionNormal>();
			eyeVBO->setData(verts.size(), &verts[0], GL_STATIC_DRAW);
		}

		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, glm::value_ptr(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f)));
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, glm::value_ptr(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f)));
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, glm::value_ptr(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f)));
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0.0f);


		glDisable(GL_BLEND);
		glEnable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_CULL_FACE);
		glEnable(GL_LIGHT0);
		glShadeModel(GL_SMOOTH);
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glColor4f(0.5f, 0.5f, 0.5f, 1);
		faceVBO->bind();
		faceVBO->setPointer();
		glTranslatef(0, 0, 0.025f);
		glDrawArrays(GL_TRIANGLES, 0, faceVBO->getLength());
		glTranslatef(0, 0, -0.025f);
		faceVBO->unsetPointer();
		faceVBO->unBind();

		eyeVBO->bind();
		eyeVBO->setPointer();
		glColor4f(1, 0, 0, 1);
		glTranslatef(0, -0.04f, 0);
		glDrawArrays(GL_TRIANGLES, 0, eyeVBO->getLength());
		glTranslatef(0, 0.08f, 0);
		glDrawArrays(GL_TRIANGLES, 0, eyeVBO->getLength());
		eyeVBO->unsetPointer();
		eyeVBO->unBind();




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
