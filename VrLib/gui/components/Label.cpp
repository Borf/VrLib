#include "Label.h"

namespace vrlib
{
	namespace gui
	{
		namespace components
		{


			Label::Label(const std::string &text, const glm::vec2 &position)
			{
				this->text = text;
				this->position = position;
			}

		}
	}
}