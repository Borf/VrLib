#pragma once

#include <functional>
#include <vector>
#include <VrLib/gui/components/Component.h>

namespace vrlib
{
	namespace gui
	{
		namespace components
		{
			class ContainerComponent : public Component, public std::vector<Component*>
			{
			public:
				virtual void draw(const glm::mat4 &parentMatrix) override;

			};
		}
	}
}