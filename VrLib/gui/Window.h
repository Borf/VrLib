#pragma once

#include <string>
#include <glm/glm.hpp>

namespace vrlib
{
	class Model;
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
			static float thickness;

			Window(const std::string &title);


			void draw(const glm::mat4& projectionMatrix, const glm::mat4 &viewMatrix);
			void setSelector(const vrlib::math::Ray& ray);
			void mouseDown();
			void mouseUp();
		};
	}
}