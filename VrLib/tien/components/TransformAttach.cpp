#include "TransformAttach.h"
#include "Transform.h"
#include <VrLib/tien/Node.h>

namespace vrlib
{
	namespace tien
	{
		namespace components
		{
			TransformAttach::TransformAttach(const vrlib::PositionalDevice &device) : device(device)
			{
			}


			TransformAttach::~TransformAttach()
			{
			}

			void TransformAttach::update(float elapsedTime)
			{
				glm::mat4 mat = device.getData();

				glm::vec3 pos(mat * glm::vec4(0, 0, 0, 1));

				node->getComponent<Transform>()->setGlobalPosition(pos);
				node->getComponent<Transform>()->setGlobalRotation(glm::quat(mat));
			}


		}
	}
}