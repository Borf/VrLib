#pragma once

#include <glm/mat4x4.hpp>
#include "Renderer.h"
#include "Scene.h"

namespace vrlib
{
	namespace tien
	{
		class Renderer;
		class Scene;

		class Tien
		{
			Scene currentRunningScene;

			enum class PlayState
			{
				Stopped,
				Playing,
				Paused,
			} playState;

		public:
			Tien();


			Renderer renderer;
			Scene currentScene;

			void start();
			void pause();
			void stop();

			void init();
			void update(float elapsedTime);
			void render(const glm::mat4 &projectionMatrix, const glm::mat4& modelViewMatrix);
		};
	}
}