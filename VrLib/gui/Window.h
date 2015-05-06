#pragma once

#include <string>
#include <glm/glm.hpp>

namespace vrlib
{
	class Model;
	class Font;
	namespace gl { class ShaderProgram; }
	namespace math { class Ray;  }
	namespace gui
	{
		namespace components { class Panel; }

		class Window
		{
		protected:
			components::Panel* rootPanel;
			std::string title;
			glm::vec2 size;
		public:
			glm::mat4 renderMatrix;

			static vrlib::Model* panelModel;
			static vrlib::gl::ShaderProgram* shader;
			static vrlib::Font* font;
			static float thickness;

			Window(const std::string &title);

			template<class T>
			T* getComponent(const std::string &name)
			{
				if (rootPanel)
					return static_cast<T*>(rootPanel->getComponent(name));
				return NULL;
			}


			void draw(const glm::mat4& projectionMatrix, const glm::mat4 &viewMatrix);
			void setSelector(const vrlib::math::Ray& ray);
			void setRootPanel(components::Panel* panel);
			void mouseDown();
			void mouseUp();
		};
	}
}