#pragma once

#include "Component.h"

#include <string>

namespace vrlib
{
	namespace gui
	{
		namespace components
		{
			class Label : public Component
			{
			public:
				std::string text;

				Label(const std::string &text);
			};
		}
	}
}