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
				template<class T = Component>
				T* getComponent(const std::string &name)
				{
					return static_cast<T*>(getComponent_internal(name));
				}


				Component* getComponent_internal(const std::string &name);
				void setComponent(const std::string &name, Component* component);

				void foreach(const std::function<void(Component*)> &callback);
				virtual void foreachWithMatrix(const std::function<void(const glm::mat4 &matrix, Component*)> &callback, const glm::mat4 &parentMatrix = glm::mat4());
			};
		}
	}
}