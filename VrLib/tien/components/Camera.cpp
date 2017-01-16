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

			Camera::Camera(const vrlib::json::Value & json)
			{
				if (json.isMember("useFbo"))
					useFbo = json["useFbo"];
				if (useFbo)
					fboSize = glm::ivec2(json["fboSize"][0].asInt(), json["fboSize"][1].asInt());
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
				ret["useFbo"] = useFbo;
				if (useFbo)
				{
					ret["fboSize"].push_back(fboSize[0]);
					ret["fboSize"].push_back(fboSize[1]);
				}
				return ret;
			}
			void Camera::buildEditor(EditorBuilder * builder, bool folded)
			{
				builder->addTitle("Camera");
			}
		}
	}
}