#include "Tien.h"
#include "Renderer.h"
#include "Scene.h"

#include <glm/gtc/matrix_transform.hpp>
#include <VrLib/gl/FBO.h>
#include <VrLib/Log.h>
#include <VrLib/json.h>

using vrlib::logger;


namespace vrlib
{
	namespace tien
	{
		Tien::Tien()
		{
			playState = PlayState::Stopped;
		}

		void Tien::init()
		{
			scene.init();
			renderer.init();
		}


		void Tien::update(float elapsedTime)
		{
#ifdef WIN32
			if ((GetAsyncKeyState(VK_F1) & 1) == 1)
				renderer.drawPhysicsDebug = !renderer.drawPhysicsDebug;
			if ((GetAsyncKeyState(VK_F2) & 1) == 1)
				renderer.drawLightDebug = !renderer.drawLightDebug;
			if ((GetAsyncKeyState(VK_F3) & 1) == 1)
				logger<<scene.asJson() << Log::newline;
#endif

			if (playState == PlayState::Playing)
				scene.update(elapsedTime);
		}

		void Tien::render(const glm::mat4 &projectionMatrix, const glm::mat4& modelViewMatrix)
		{
			if (playState == PlayState::Playing)
				renderer.render(scene, projectionMatrix, modelViewMatrix);
		}


		void Tien::start()
		{
			playState = PlayState::Playing;
		}

		/*void Tien::stop()
		{
			throw "not implemented yet";
		}*/
		void Tien::pause()
		{
			playState = PlayState::Paused;
		}

		void Tien::saveCubeMap(const glm::vec3 position, const std::string &fileName)
		{
			vrlib::gl::FBO* fbo = new vrlib::gl::FBO(1024, 1024, true);
			struct CameraDirection
			{
				GLenum cubemapFace;
				glm::vec3 target;
				glm::vec3 up;
			};

			CameraDirection gCameraDirections[6] =
			{
				{ GL_TEXTURE_CUBE_MAP_POSITIVE_X, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f) },
				{ GL_TEXTURE_CUBE_MAP_NEGATIVE_X, glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f) },
				{ GL_TEXTURE_CUBE_MAP_POSITIVE_Y, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f) },
				{ GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f) },
				{ GL_TEXTURE_CUBE_MAP_POSITIVE_Z, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f) },
				{ GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f) }
			};

			glViewport(0, 0, fbo->getWidth(), fbo->getHeight());

			for (int i = 0; i < 6; i++)
			{
				fbo->bind();
				render(glm::perspective(45.0f, 1.0f, 0.1f, 500.0f), glm::lookAt(position, position + gCameraDirections[i].target, gCameraDirections[i].up));
				fbo->unbind();
				fbo->saveAsFile(fileName + "." + std::to_string(i) + ".png");
			}


			delete fbo;

		}


	}
}