#include "Camera.h"

#include <VrLib/gl/FBO.h>
#include <VrLib/json.h>

namespace vrlib
{
	namespace tien
	{
		namespace components
		{
			Camera::Camera()
			{

			}

			void Camera::render()
			{

			}
			json::Value Camera::toJson() const
			{
				json::Value ret;
				ret["type"] = "camera";
				return ret;
			}
		}
	}
}