#pragma once

#include <functional>

#include "Component.h"

namespace vrlib
{
	class Model;
	namespace gui
	{
		namespace components
		{
			class CheckBox : public Component
			{
			public:
				struct Models
				{
					vrlib::Model* on = NULL;
					vrlib::Model* off = NULL;
				};

				static Models models;
				CheckBox(bool initialValue = false, const glm::vec2 &position = glm::vec2(0,0), const std::function<void()> &callback = nullptr);

				bool value;
				virtual void draw(const glm::mat4 &parentMatrix) override;
			};
		}
	}
}