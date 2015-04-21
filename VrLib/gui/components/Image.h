#pragma once

#include "Component.h"

namespace vrlib
{
	namespace gui
	{
		namespace components
		{
			class Image : public Component
			{
			public:
				Image(vrlib::Texture* texture);
			};
		}
	}
}