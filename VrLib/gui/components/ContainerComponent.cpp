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
		}
	}
}