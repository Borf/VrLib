#pragma once

#include <functional>
#include <string>

#include "Component.h"

namespace vrlib
{
	namespace gui
	{
		namespace components
		{
			class Button : public Component
			{
			public:
				Button(const std::string &text, std::function<void()> callback = nullptr);
			};
		}
	}
}