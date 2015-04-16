#pragma once

#include <glm/glm.hpp>

#include <list>

namespace vrlib
{
	class User;
	class Application;
	class Kernel;
	namespace json { class Value; }

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


		static Viewport* createViewport(Kernel* kernel, json::Value viewportConfig, json::Value otherConfigs);
	};


}
