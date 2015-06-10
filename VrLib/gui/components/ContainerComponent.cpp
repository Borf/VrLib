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

			Component* ContainerComponent::getComponent_internal(const std::string &name)
			{
				Component* subEl = NULL;

				for (auto el : *this)
					if (el->name == name)
						return el;
					else if (dynamic_cast<ContainerComponent*>(el))
						if (subEl = dynamic_cast<ContainerComponent*>(el)->getComponent_internal(name))
							return subEl;
				return NULL;
			}


			void ContainerComponent::setComponent(const std::string &name, Component* component)
			{
				for (size_t i = 0; i < std::vector<Component*>::size(); i++)
					if ((*this)[i]->name == name)
						(*this)[i] = component;
					else if (dynamic_cast<ContainerComponent*>((*this)[i]))
						dynamic_cast<ContainerComponent*>((*this)[i])->setComponent(name, component);
			}


			void ContainerComponent::foreach(const std::function<void(Component*)> &callback)
			{
				callback(this);
				for (auto el : *this)
				{
					if (dynamic_cast<ContainerComponent*>(el))
						dynamic_cast<ContainerComponent*>(el)->foreach(callback);
					else
						callback(el);
				}
			}

			void ContainerComponent::foreachWithMatrix(const std::function<void(const glm::mat4 &parentMatrix, Component*)> &callback, const glm::mat4 &parentMatrix)
			{
				glm::mat4 matrix = glm::translate(parentMatrix, glm::vec3(position.x, position.y, 0));
				callback(matrix, this);
				for (auto el : *this)
				{
					if (dynamic_cast<ContainerComponent*>(el))
						dynamic_cast<ContainerComponent*>(el)->foreachWithMatrix(callback, matrix);
					else
						callback(matrix, el);
				}
			}

		}
	}
}