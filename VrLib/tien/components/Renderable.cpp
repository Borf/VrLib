#include "Renderable.h"

#include "../Node.h"
#include "../Scene.h"

namespace vrlib
{
	namespace tien
	{
		namespace components
		{
			Renderable::~Renderable()
			{
				node->setTreeDirty(nullptr, nullptr);
			}

		}
	}
}