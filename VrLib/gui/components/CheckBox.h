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
				static vrlib::Model* checkboxModel;
				CheckBox(bool initialValue, const glm::vec2 &position, const std::function<void()> &callback = nullptr);

				bool value;
				virtual void draw(const glm::mat4 &parentMatrix) override;
			};
		}
	}
}