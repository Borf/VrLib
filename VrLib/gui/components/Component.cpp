#include "Component.h"


namespace vrlib
{
	namespace gui
	{
		namespace components
		{


			void Component::addClickHandler(const std::function<void(void)>& callback)
			{

			}

			void Component::setBounds(const glm::vec2 &position, const glm::vec2 &size)
			{
				this->position = position;
				this->size = size;
			}

		}
	}
}