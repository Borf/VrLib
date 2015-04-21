#pragma once

#include <functional>

namespace vrlib
{
	namespace gui
	{
		namespace components
		{
			class Component
			{
			public:

				void addClickHandler(const std::function<void(void)>& callback);
			};
		}
	}
}