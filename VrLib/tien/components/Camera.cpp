#include "Camera.h"

#include <VrLib/gl/FBO.h>
#include <VrLib/json.h>

#include <VrLib/tien/Node.h>
#include <VrLib/tien/Scene.h>

namespace vrlib
{
	namespace tien
	{
		namespace components
		{
			Camera::Camera()
			{

			}

			Camera::~Camera()
			{
				if (node->getScene().cameraNode == node)
				{
					logger << "Deleting main camera node" << Log::newline;
					node->getScene().cameraNode = nullptr;
				}
			}

			void Camera::render()
			{

			}
			json::Value Camera::toJson(json::Value &meshes) const
			{
				json::Value ret;
				ret["type"] = "camera";
				return ret;
			}
		}
	}
}