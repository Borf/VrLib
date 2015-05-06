#include "ContainerComponent.h"
#include <glm/gtc/matrix_transform.hpp>

namespace vrlib
{
	namespace gui
	{
		namespace components
		{




			void ContainerComponent::draw(const glm::mat4 &parentMatrix)
			{
				glm::mat4 matrix = glm::translate(parentMatrix, glm::vec3(position.x, position.y, 0));
				for (auto c : *this)
					c->draw(matrix);
			}

			Component* ContainerComponent::getComponent(const std::string &name)
			{
				Component* subEl = NULL;

				for (auto el : *this)
					if (el->name == name)
						return el;
					else if (static_cast<ContainerComponent*>(el))
						if(subEl = static_cast<ContainerComponent*>(el)->getComponent(name))
							return subEl;
				return NULL;
			}

		}
	}
}