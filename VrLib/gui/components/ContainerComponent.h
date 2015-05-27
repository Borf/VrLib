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
				Component* getComponent(const std::string &name);

				void foreach(const std::function<void(Component*)> &callback);
				virtual void foreachWithMatrix(const std::function<void(const glm::mat4 &matrix, Component*)> &callback, const glm::mat4 &parentMatrix = glm::mat4());
			};
		}
	}
}