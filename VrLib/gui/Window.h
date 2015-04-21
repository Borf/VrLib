#pragma once

#include <string>
#include <glm/glm.hpp>

namespace vrlib
{
	namespace components { class Panel; }
	namespace math { class Ray;  }
	namespace gui
	{
		class Window
		{
		protected:
			components::Panel* rootPanel;
			std::string title;
		public:
			glm::mat4 renderMatrix;


			Window(const std::string &title);


			void draw();


			void setSelector(const vrlib::math::Ray& ray);
			void mouseDown();
			void mouseUp();
		};
	}
}