#include "Camera.h"

#include <VrLib/gl/FBO.h>
#include <VrLib/json.hpp>

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

			Camera::Camera(const json & data)
			{
				if (data.find("useFbo") != data.end())
					useFbo = data["useFbo"];
				if (useFbo)
					fboSize = glm::ivec2(data["fboSize"][0], data["fboSize"][1]);
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
			json Camera::toJson(json &meshes) const
			{
				json ret;
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