#pragma once

#include "Component.h"

namespace vrlib
{
	namespace gui
	{
		namespace layoutmanagers { class LayoutManager; }
		namespace components
		{
			class Panel : public Component
			{
			public:
				Panel(layoutmanagers::LayoutManager* layoutManager);

				void add(Component* component);
			};
		}
	}
}