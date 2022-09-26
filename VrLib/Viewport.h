#pragma once

#include <glm/glm.hpp>
#include <VrLib/json.hpp>
#include <list>

namespace vrlib
{
	class User;
	class Application;
	class Kernel;

	class Viewport
	{
	protected:
		glm::vec2 position;
		glm::vec2 size;
		User* user;
	public:
		Viewport(User* user);
		~Viewport();
		virtual void setScreenPosition(float x, float y);
		virtual void setScreenSize(float x, float y);

		inline float x() { return position.x; }
		inline float y() { return position.y; }
		inline float width() { return size.x; }
		inline float height() { return size.y; }


		virtual void draw(Application* application) = 0;
		virtual glm::mat4 getProjectionMatrix() = 0;

		void resetOpenGL();

		static Viewport* createViewport(Kernel* kernel, nlohmann::json viewportConfig, nlohmann::json otherConfigs);
	};


}
