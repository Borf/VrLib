#pragma once

#include "../Component.h"

namespace vrlib
{
	namespace tien
	{
		namespace components
		{
			class PostProcessor : public Component
			{
			public:
				virtual void process() = 0;
			};
		}
	}
}