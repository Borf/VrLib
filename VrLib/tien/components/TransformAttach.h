#pragma once

#include "../Component.h"
#include <VrLib/Device.h>


class btPoint2PointConstraint;
class btDiscreteDynamicsWorld;

namespace vrlib
{
	class PositionalDevice;

	namespace tien
	{
		class Renderer;

		namespace components
		{
			class TransformAttach : public Component
			{
				const vrlib::PositionalDevice &device;
			public:
				glm::mat4 offset;

				TransformAttach(const vrlib::PositionalDevice &device);
				~TransformAttach();
				nlohmann::json toJson(nlohmann::json &meshes) const override;

				void postUpdate(Scene& scene) override;
			};
		}
	}
}