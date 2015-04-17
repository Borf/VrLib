#pragma once

#include "LayoutManager.h"

namespace vrlib
{
	namespace gui
	{
		namespace layoutmanagers
		{
			class TableLayout : public LayoutManager
			{
			public:
				TableLayout(int cols);
			};
		}
	}
}