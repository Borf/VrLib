#include "Tien.h"
#include "Renderer.h"
#include "Scene.h"


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
			renderer.init();
		}


		void Tien::update(float elapsedTime)
		{
#ifdef WIN32
			SHORT debugKey = GetAsyncKeyState(VK_F1);
			if ((debugKey & 1) == 1)
				renderer.drawPhysicsDebug = !renderer.drawPhysicsDebug;
#endif

			if (playState == PlayState::Playing)
				currentRunningScene.update(elapsedTime);
		}

		void Tien::render(const glm::mat4 &projectionMatrix, const glm::mat4& modelViewMatrix)
		{
			if (playState == PlayState::Stopped)
				renderer.render(currentScene, projectionMatrix, modelViewMatrix);
			else
				renderer.render(currentRunningScene, projectionMatrix, modelViewMatrix);
		}


		void Tien::start()
		{
			if (playState == PlayState::Stopped)
			{
				currentRunningScene = Scene(currentScene);
				currentRunningScene.prepareForRun();
			}
			
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


	}
}